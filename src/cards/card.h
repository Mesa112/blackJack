#ifndef CARD_H
#define CARD_H

#include <iostream>

enum Suit {Hearts, Diamonds, Clubs, Spades};

class Card{
    private:
        int value;
        Suit suit;
    public:
        Card(int v, Suit s);
        int getValue() const;
        Suit getSuit() const;
        std :: string toString() const;

};

#endif
