#ifndef DECKS_HPP
#define DECKS_HPP

#include <random>
#include <vector>

enum class Rank {
    twoLt = 1, oneLt, cpt, maj, ltc, col, bg, radm, vadm, adm, sadm, sd
};

class Deck {
public:
    Deck(int, Rank, std::vector<int>);
    int getLeader() const { return leaderId; }
    Rank getLeaderRank() const { return leaderRank; }
    const std::vector<int>& getDeck() const { return cards; }
private:
    int leaderId;
    Rank leaderRank;
    std::vector<int> cards;
};

class CardPool {
public:
    CardPool(std::vector<int>, int);
    int getRandomCard(std::mt19937_64&, const std::vector<int>&) const;
    int getSampleSize() const { return sampleSize; }
private:
    std::vector<int> poolVec;
    int sampleSize;
};

class DeckFactory {
public:
    DeckFactory(std::vector<CardPool>);
    Deck generateDeck(std::mt19937_64&) const;
private:
    std::vector<CardPool> pools;
};

int deckCost(const Deck&, const std::vector<int>&);
DeckFactory createDefaultFactory(const std::vector<int>&);

#endif
