#include "Dealer.h"
#include <iostream>
#include <stdexcept>

Dealer::Dealer() : Player("Dealer", true), holeCardRevealed(false) {}

bool Dealer::shouldHit() const {
    return getTotalValue() <= 16;
}

void Dealer::revealHoleCard() {
    holeCardRevealed = true;
}

bool Dealer::isHoleCardRevealed() const {
    return holeCardRevealed;
}

Card Dealer::getHoleCard() const {
    // The hole card is typically the second card (index 1)
    if (getCardCount() > 1) {
        return getCard(1);
    }
    throw std::runtime_error("No hole card available");
}

bool Dealer::checkForBlackjack() const {
    return isBlackjack();
}

void Dealer::resetForNewGame() {
    holeCardRevealed = false;
}
