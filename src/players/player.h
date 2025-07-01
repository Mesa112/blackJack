#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "hand.h"
#include "deck.h"

class Player {
private:
    std::string name;
    Hand hand;
    bool isDealer;

public:

    Player(const std::string& playerName, bool dealer = false);
    void addCard(const Card& card);
    void clearHand();
    int getTotalValue() const;
    Card getUpCard() const;
    Card getCard(int index) const;
    size_t getCardCount() const;
    bool isBlackjack() const;
    bool isBusted() const;
    std::string getName() const;
    bool getIsDealer() const;
    void removeCard(size_t index);
};

#endif