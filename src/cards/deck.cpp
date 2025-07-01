#include "deck.h"

#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>

Deck::Deck(){
    resetDeck();
}

void Deck::shuffle(){
    static std::mt19937 rng(static_cast<unsigned int>(std::time(0)));
    std::shuffle(cards.begin(), cards.end(), rng);
}

Card Deck::dealCard(){
    if(cards.empty()){
        throw std::out_of_range("No cards left in the deck");
    }

    Card dealtCard = cards.back();
    cards.pop_back();

    if(isThresholdReached()){
        cutDeck();
    }
    return dealtCard;
}

bool Deck::isThresholdReached() const {
    return cards.size() <= static_cast<size_t>(cutPoint);
}

void Deck::cutDeck(){
    resetDeck();
}

void Deck::resetDeck(){
    cards.clear();
    for(int deck = 0; deck < num_decks; ++deck){
        for(int suit = Hearts; suit <= Spades; ++suit){
            for(int value = 1; value <= 13; ++value){
                cards.emplace_back(value, static_cast<Suit>(suit));
            }
        }
    }
    shuffle();
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<size_t> dist(60, 80);
    cutPoint = dist(rng);

    std::cout << "New " << num_decks << "-deck shoe created (" << total_cards << " cards). Cut point at " 
              << cutPoint << " cards remaining.\n";
}

int Deck::getCardsRemaining() const {
    return static_cast<int>(cards.size());
}