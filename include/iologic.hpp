#ifndef IOLOGIC_HPP
#define IOLOGIC_HPP

#include <cstdint>

#include <QFile>

#include "decks.hpp"

bool isFileValid(QFile*);
std::vector<int> readDeckCosts(QFile*);
void writeDeck(QFile*, const Deck&, qint64);
void createStarterDecks(QFile*, const DeckFactory&,
                        const std::vector<int>&, uint64_t);
void writeDmkAi(QFile*, bool);
void toggleDcCheck(QFile*, bool);

#endif
