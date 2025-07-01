#include "card.h"

#include <iostream>

Card::Card(int v, Suit s) : value(v), suit(s) {}

int Card::getValue() const {
    return value;
}

std::string Card::toString() const {
    static const std::string suitNames[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
    static const std::string valueNames[] = { "", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
    return valueNames[value] + " of " + suitNames[suit];
}

Suit Card::getSuit() const {
    return suit;
}
