#ifndef DECK_H
#define DECK_H

#include <vector>
#include <iostream>
#include "card.h"

class Deck {
private:
    std::vector<Card> cards;
    int cutPoint; 
    static const int num_decks = 8;
    static const int cards_per_deck = 52;
    static const int total_cards = num_decks * cards_per_deck;

public:
    Deck();
    //Deck actions below
    void shuffle();
    Card dealCard();
    bool isThresholdReached() const; 
    void cutDeck();                  
    void resetDeck();                
    int getCardsRemaining() const;
};

#endif
