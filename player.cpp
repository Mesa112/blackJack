#include "player.h"
#include "hand.h"
#include <iostream>

Player::Player(const std::string& playerName, bool dealer) 
    : name(playerName), hand(), isDealer(dealer) {}

void Player::addCard(const Card& card) {
    hand.addCard(card);
}

void Player::clearHand() {
    hand.clearHand();
}

int Player::getTotalValue() const {
    return hand.getTotalValue();
}

Card Player::getUpCard() const{
    // The upcard will be the first card dealt which is the norm in blackjack so index 0

    if(hand.getTotalValue() > 0) return hand.getCard(0);

    throw std::runtime_error("No cards in current hand");
}

Card Player::getCard(int index) const{
    return hand.getCard(index);
}

size_t Player::getCardCount() const{
    return hand.getCardCount();
}

bool Player::isBlackjack() const{
    return hand.isBlackjack();
}

bool Player::isBusted() const{
    return hand.isBusted();
}

std::string Player::getName() const{
    return name;
}

bool Player::getIsDealer() const{
    return isDealer;
}

void Player::removeCard(size_t index) {
    if (index < hand.getCardCount()) {
        hand.removeCard(index);
    }
}

