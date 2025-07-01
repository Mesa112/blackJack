#ifndef HAND_H
#define HAND_H

#include <vector>
#include "card.h"

class Hand {
private:
    std::vector<Card> cards;

public:
    Hand();
    
    void addCard(const Card& card);
    void clearHand();
    int getTotalValue() const;
    bool isBlackjack() const;
    bool isBusted() const;
    Card getCard(int index) const;
    size_t getCardCount() const;
    void removeCard(size_t index);
};

#endif
