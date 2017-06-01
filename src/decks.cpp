#include <algorithm>
#include <stdexcept>

#include "decks.hpp"

const int minCardId = 0;
const int maxCardId = 853;
const int maxMonId = 682;
const int exodiaId = 671;
const int deckSize = 40;
const int maxCopyCount = 3;
const int minExodiaId = 54;
const int maxExodiaId = 58;

Deck::Deck(int leader, Rank rank, std::vector<int> deck)
{
    if ((leader < minCardId) || (leader == exodiaId) || (leader > maxMonId))
        throw std::invalid_argument("Leader not a monster.");

    if (deck.size() != deckSize)
        throw std::invalid_argument("Deck incorrect size.");

    for (int c : deck) {
        if ((c < minCardId) || (c == exodiaId) || (c > maxCardId))
            throw std::invalid_argument("Deck contains invalid cards.");
    }

    for (int i = minCardId; i <= maxCardId; ++i) {
        int tmpCount = std::count(deck.begin(), deck.end(), i);
        if ((tmpCount > maxCopyCount)
                || ((tmpCount > 1) && (i >= minExodiaId) && (i <= maxExodiaId)))
            throw std::invalid_argument("Deck contains too many duplicates.");
    }

    std::sort(deck.begin(), deck.end());

    leaderId = leader;
    leaderRank = rank;
    cards = deck;
}

// TODO: Be careful with checking of sum and overflow.
// TODO: Check if the pool is capable of producing the right number of cards.
CardPool::CardPool(std::vector<int> newPoolVec, int newSampleSize)
{
    if (newPoolVec.size() != (maxCardId + 1))
        throw std::invalid_argument("Pool wrong size.");

    int sum = 0;
    for (int i = minCardId; i <= maxCardId; ++i) {
        if (newPoolVec[i] < 0)
            throw std::invalid_argument("Pool has negative probabilities.");
        if ((i == exodiaId) && (newPoolVec[i] > 0))
            throw std::invalid_argument("Pool can drop Summoned Lord Exodia.");
        sum += newPoolVec[i];
    }
    if (sum == 0)
        throw std::invalid_argument("Pool is all zeroes.");

    if ((newSampleSize <= 0) || (newSampleSize > deckSize))
        throw std::invalid_argument("Invalid sample size.");

    poolVec = newPoolVec;
    sampleSize = newSampleSize;
}

// TODO: Be careful with total sum and overflow.
// TODO: Clean up looping over the vector.
int CardPool::getRandomCard(std::mt19937_64& prng,
                            const std::vector<int>& bannedCards) const
{
    int sum = 0;
    for (int i = minCardId; i <= maxCardId; ++i) {
        if (std::find(bannedCards.begin(), bannedCards.end(), i)
                == bannedCards.end())
            sum += poolVec[i];
    }
    if (sum == 0)
        throw std::invalid_argument("No remaining valid cards.");

    std::uniform_int_distribution<int> dis(0, sum - 1);
    int randomNumb = dis(prng);

    sum = 0;
    for (int i = minCardId; i <= maxCardId; ++i) {
        if (std::find(bannedCards.begin(), bannedCards.end(), i)
                == bannedCards.end())
            sum += poolVec[i];
        if (sum > randomNumb)
            return i;
    }

    throw std::logic_error("Impossible error.");
}

// TODO: Consider if I want to make the deck pools disjoint.
DeckFactory::DeckFactory(std::vector<CardPool> newPools)
{
    if (newPools.size() < 2)
        throw std::invalid_argument("Not enough pools.");
    if (newPools[0].getSampleSize() != 1)
        throw std::invalid_argument("First pool not fit for DL.");

    int sum = 0;
    for (unsigned int i = 1; i < newPools.size(); ++i)
        sum += newPools[i].getSampleSize();
    if (sum != deckSize)
        throw std::invalid_argument("Pools do not together form a deck.");

    pools = newPools;
}

// TODO: Change this when I improve getRandomCard to ignore certain cards.
Deck DeckFactory::generateDeck(std::mt19937_64& prng) const
{
    int leader = pools[0].getRandomCard(prng, {});
    Rank rank = Rank::twoLt;
    std::vector<int> cards;
    std::vector<int> bannedCards;

    for (unsigned int i = 1; i < pools.size(); ++i) {
        for (int j = 0; j < pools[i].getSampleSize(); ++j) {
            int newCard = pools[i].getRandomCard(prng, bannedCards);
            if (std::count(cards.begin(), cards.end(), newCard) == 2)
                bannedCards.push_back(newCard);
            else if ((newCard >= minExodiaId) && (newCard <= maxExodiaId))
                bannedCards.push_back(newCard);
            cards.push_back(newCard);
        }
    }

    return Deck(leader, rank, cards);
}

int deckCost(const Deck& deck, const std::vector<int>& deckCostVec)
{
    if (deckCostVec.size() != (maxCardId + 1))
        throw std::invalid_argument("Invalid DC vector.");

    int dc = 0;
    for (int i : deck.getDeck())
        dc += deckCostVec[i];
    return dc;
}

DeckFactory createDefaultFactory(const std::vector<int>& dcs)
{
    std::vector<int> dlPool(854, 0);
    std::vector<int> trapPool(854, 0);
    std::vector<int> equipPool(854, 0);
    std::vector<int> fieldPool(854, 0);
    std::vector<int> magicPool(854, 0);
    std::vector<int> monOnePool(854, 0);
    std::vector<int> monTwoPool(854, 0);
    std::vector<int> monThreePool(854, 0);
    std::vector<int> monFourPool(854, 0);

    for (int i = 0; i < 683; ++i) {
        if (i != 671) {
            dlPool[i] = 1;
            if (dcs[i] < 20)
                monFourPool[i] = 1;
            else if (dcs[i] < 25)
                monThreePool[i] = 1;
            else if (dcs[i] < 30)
                monTwoPool[i] = 1;
            else if (dcs[i] < 40)
                monOnePool[i] = 1;
        }
    }

    for (int i = 801; i < 814; ++i) {
        if (dcs[i] < 60)
            trapPool[i] = 1;
    }

    for (int i = 752; i < 801; ++i) {
        if (dcs[i] <= 15)
            equipPool[i] = 1;
    }

    for (int i = 689; i < 697; ++i)
        fieldPool[i] = 1;

    for (int i = 699; i < 750; ++i) {
        if ((dcs[i] >= 20) && (dcs[i] <= 50))
            magicPool[i] = 1;
    }

    DeckFactory factory({CardPool(dlPool, 1),
                         CardPool(trapPool, 1),
                         CardPool(equipPool, 2),
                         CardPool(fieldPool, 1),
                         CardPool(magicPool, 1),
                         CardPool(monOnePool, 5),
                         CardPool(monTwoPool, 5),
                         CardPool(monThreePool, 5),
                         CardPool(monFourPool, 20)});
    return factory;
}
