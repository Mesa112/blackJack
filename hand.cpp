#include "hand.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>

Hand::Hand() {}

void Hand::addCard(const Card& card) {
    cards.push_back(card);
}

void Hand::clearHand() {
    cards.clear();
}

int Hand::getTotalValue() const{
    int totalVal = 0;
    int aceCount = 0;
    for (const auto& card : cards) {
        int cardValue = card.getValue();
        if (cardValue == 1) { // we consider ace as 1
            aceCount++;
            totalVal += 11; // initally we count ace as 11
        } else if (cardValue > 10) { // consider J, Q, K as 10
            totalVal += 10;
        } else {
            totalVal += cardValue;
        }
    }
    // now we need to adjust for Aces if totalVal > 21
    while(totalVal > 21 && aceCount > 0) {
        totalVal -= 10; // Count Ace as 1 instead of 11
        aceCount--;
    }
    return totalVal;
}

bool Hand::isBlackjack() const {
    return cards.size() == 2 && getTotalValue() == 21;
}

bool Hand::isBusted() const {
    return getTotalValue() > 21;
}

Card Hand::getCard(int index) const{
    if(index >= 0 && static_cast<size_t>(index) < getCardCount()){
        return cards[index];
    }
    throw std::out_of_range("Invalid card index");
}

size_t Hand::getCardCount() const{
    return cards.size();
}

void Hand::removeCard(size_t index) {
    if (index < cards.size()) {
        cards.erase(cards.begin() + index);
        getTotalValue();
    }
}