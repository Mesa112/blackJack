#ifndef DEALER_H
#define DEALER_H

#include "player.h"

class Dealer : public Player {
private:
    bool holeCardRevealed;

public:
    Dealer();
    
    bool shouldHit() const;           // True if dealer must hit (total <= 16)
    void revealHoleCard();            // Shows the hole card
    bool checkForBlackjack() const;   // Early blackjack check
    void playTurn();                  // Dealer's automatic play logic
    bool isHoleCardRevealed() const;
    Card getHoleCard() const;
    void resetForNewGame();
};

#endif