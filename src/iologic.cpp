#include "iologic.hpp"

const qint64 correctSize = 2112782336;
const qint64 cardDataLoc = 0x28F180;
const qint64 starterDecksLoc = 0x2A0A70;
const int numbOfDecks = 17;
const int maxDc = 854;

const char defaultAis[] = {
    -112, 124, 17, 0, 0, 15, 17, 0,
    48, -60, 17, 0, 48, 22, 18, 0,
    -80, 106, 18, 0, -96, -71, 18, 0,
    -112, 14, 19, 0, 96, 80, 19, 0,
    -64, -106, 19, 0, -48, -38, 19, 0,
    -32, 34, 20, 0, -112, 103, 20, 0,
    48, -73, 20, 0, 112, -5, 20, 0,
    64, 64, 21, 0, 16, 66, 21, 0,
    -80, -118, 21, 0, -128, -47, 21, 0,
    96, 47, 22, 0, -32, 118, 22, 0,
    -16, -79, 22, 0, 112, 35, 23, 0
};
const char dmkAi[] = {112, 35, 23, 0};
const QByteArray aiByteString(QByteArray::fromRawData(defaultAis, 88));
const QByteArray dmkByteString(QByteArray::fromRawData(dmkAi, 4));
const qint64 aiMapLoc = 0x28AFB4;
const int numbOfDuelists = 22;

const char dcCheck[] = {42, 8, 67, 0};
const char noDcCheck[] = {40, 14, 0, 112};
const QByteArray dcCheckBytes(QByteArray::fromRawData(dcCheck, 4));
const QByteArray noDcCheckBytes(QByteArray::fromRawData(noDcCheck, 4));
const qint64 dcCheckLoc = 0x209F40;

// TODO: Figure out a better check than just file size that doesn't
// cause a freeze like checking the hash here does.
bool isFileValid(QFile* dotrIso)
{
    return dotrIso->size() == correctSize;
}

std::vector<int> readDeckCosts(QFile* dotrIso)
{
    qint64 address = cardDataLoc + 3;
    std::vector<int> dcs;
    for (int i = 0; i < 854; ++i) {
        dotrIso->seek(address + 20 * i);
        QByteArray dcByte = dotrIso->read(1);
        dcs.push_back(static_cast<int>(dcByte[0]));
    }
    return dcs;
}

void writeDeck(QFile* dotrIso, const Deck& deck, qint64 address)
{
    if (dotrIso == nullptr)
        throw std::invalid_argument("Null file.");

    QByteArray deckBytes;

    int leaderNumb = (deck.getLeader())
            | (static_cast<int>(deck.getLeaderRank()) << 12);
    deckBytes.append(leaderNumb & 0xFF);
    deckBytes.append((leaderNumb >> 8) & 0xFF);

    for (int c : deck.getDeck()) {
        deckBytes.append(c & 0xFF);
        deckBytes.append((c >> 8) & 0xFF);
    }

    dotrIso->seek(address);
    dotrIso->write(deckBytes);
}

void createStarterDecks(QFile* dotrIso, const DeckFactory& factory,
                        const std::vector<int>& deckCosts, uint64_t initSeed)
{
    if (dotrIso == nullptr)
        throw std::invalid_argument("Null file.");

    std::mt19937_64 prng(initSeed);

    bool deckMade = false;
    qint64 address = starterDecksLoc;

    for (int i = 0; i < numbOfDecks; ++i) {
        Deck newDeck = factory.generateDeck(prng);
        while (!deckMade) {
            if (deckCost(newDeck, deckCosts) <= maxDc)
                deckMade = true;
            else
                newDeck = factory.generateDeck(prng);
        }
        writeDeck(dotrIso, newDeck, address);

        deckMade = false;
        address += 82;
    }
}

void writeDmkAi(QFile* dotrIso, bool writeDmk)
{
    if (dotrIso == nullptr)
        throw std::invalid_argument("Null file.");

    if (writeDmk) {
        for (int i = 0; i < numbOfDuelists; ++i) {
            dotrIso->seek(aiMapLoc + 4 * i);
            dotrIso->write(dmkByteString);
        }
    }
    else {
        dotrIso->seek(aiMapLoc);
        dotrIso->write(aiByteString);
    }
}

void toggleDcCheck(QFile* dotrIso, bool haveDcCheck)
{
    if (dotrIso == nullptr)
        throw std::invalid_argument("Null file.");

    dotrIso->seek(dcCheckLoc);
    if (haveDcCheck)
        dotrIso->write(dcCheckBytes);
    else
        dotrIso->write(noDcCheckBytes);
}
