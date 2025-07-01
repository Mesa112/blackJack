#include <iostream>
#include <cctype>
#include "SplitHand.h"
#include "deck.h"

SplitHand::SplitHand(std::vector<Player>& gamePlayers, Deck& gameDeck, int maxSplitsAllowed) 
    : players(gamePlayers), deck(gameDeck), maxSplits(maxSplitsAllowed) {}

bool SplitHand::canPlayerSplit(int playerIndex) {
    if(playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
        return false;
    }
    Player& player = players[playerIndex];

    if(player.getCardCount() != 2) {
        return false;
    }

    Card card1 = player.getCard(0);
    Card card2 = player.getCard(1);

    int value1 = card1.getValue();
    int value2 = card2.getValue();

    // Check if player already has maximum splits
    if (playerSplitHands.find(playerIndex) != playerSplitHands.end()) {
        if (playerSplitHands[playerIndex].size() >= static_cast<size_t>(maxSplits)) {
            return false;
        }
    }

    if(value1 == value2) return true;

    bool card1isFaceCard = (value1 == 10 || value1 == 11 || value1 == 12 || value1 == 13);
    bool card2isFaceCard = (value2 == 10 || value2 == 11 || value2 == 12 || value2 == 13);

    return card1isFaceCard && card2isFaceCard;
}

void SplitHand::playSplitHands(int playerIndex) {
    Player& player = players[playerIndex];
    auto& splitHands = playerSplitHands[playerIndex];
    
    for (size_t handIndex = 0; handIndex < splitHands.size(); ++handIndex) {
        SplitHands& currentHand = splitHands[handIndex];
        
        std::cout << "\n--- " << player.getName() << "'s Hand " << (handIndex + 1) << " ---" << std::endl;
        
        // Special case: Aces are split and get only one card each
        if (currentHand.isAceSplit) {
            displaySplitHand(playerIndex, handIndex);
            if (currentHand.is21()) {
                std::cout << "21! (Note: Split hands cannot have 'blackjack')" << std::endl;
            }
            continue;  // No further action allowed on split aces
        }
        
        playSingleSplitHand(playerIndex, handIndex);
    }
}

void SplitHand::playSingleSplitHand(int playerIndex, int handIndex) {
    auto& splitHands = playerSplitHands[playerIndex];
    SplitHands& currentHand = splitHands[handIndex];
    
    displaySplitHand(playerIndex, handIndex);
    
    // Check for 21 (not blackjack on splits)
    if (currentHand.is21()) {
        std::cout << "21 on hand " << (handIndex + 1) << "!" << std::endl;
        return;
    }
    
    bool handActive = true;
    
    while (!currentHand.isBusted() && handActive && currentHand.isActive) {
        // Determine available actions
        bool canDouble = currentHand.canDouble();
        bool canResplit = canPlayerResplit(playerIndex, handIndex);
        
        std::cout << "Hand " << (handIndex + 1) << " - Choose action: (h)it, (s)tand";
        if (canDouble) std::cout << ", (d)ouble down";
        if (canResplit) std::cout << ", s(p)lit again";
        std::cout << ": ";
        
        char choice;
        std::cin >> choice;
        choice = std::tolower(choice);
        
        if (choice == 'h') {
            Card newCard = deck.dealCard();
            currentHand.addCard(newCard);
            std::cout << "Drew: " << newCard.toString() << std::endl;
            
            displaySplitHand(playerIndex, handIndex);
            
            if (currentHand.isBusted()) {
                std::cout << "Hand " << (handIndex + 1) << " busts!" << std::endl;
                handActive = false;
            }
            
        } else if (choice == 's') {
            std::cout << "Standing on hand " << (handIndex + 1) << std::endl;
            handActive = false;
            
        } else if (choice == 'd' && canDouble) {
            if (playerDoublesDownSplit(playerIndex, handIndex)) {
                handActive = false;
            }
            
        } else if (choice == 'p' && canResplit) {
            if (reSplit(playerIndex, handIndex)) {
                handActive = false;  // Current hand is done, will continue with next hands
                
                // Note: After re-splitting, the playSplitHands loop will handle the new hands
                // since we inserted the new hand into the vector, it will be processed
                // in the main loop in playSplitHands()
            }
            
        } else {
            std::cout << "Invalid choice. Available options: h, s";
            if (canDouble) std::cout << ", d";
            if (canResplit) std::cout << ", p";
            std::cout << std::endl;
        }
    }
}

bool SplitHand::canPlayerResplit(int playerIndex, int handIndex) {
    if (playerSplitHands.find(playerIndex) == playerSplitHands.end()) {
        return false;
    }
    
    auto& splitHands = playerSplitHands[playerIndex];

    if (handIndex >= static_cast<int>(splitHands.size())) {
        return false;
    }
    
    const SplitHands& hand = splitHands[handIndex];
    
    if (hand.getCardCount() != 2) {
        return false;
    }
    
    if (splitHands.size() >= static_cast<size_t>(maxSplits)) {
        return false;
    }
    
    if (hand.isAceSplit) {
        return false;
    }
    
    Card card1 = hand.cards[0];
    Card card2 = hand.cards[1];
    
    int value1 = card1.getValue();
    int value2 = card2.getValue();
    
    if (value1 == value2) return true;
    
    bool card1isFaceCard = (value1 == 10 || value1 == 11 || value1 == 12 || value1 == 13);
    bool card2isFaceCard = (value2 == 10 || value2 == 11 || value2 == 12 || value2 == 13);
    
    return card1isFaceCard && card2isFaceCard;
}

bool SplitHand::reSplit(int playerIndex, int handIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
        return false;
    }
    
    if (playerSplitHands.find(playerIndex) == playerSplitHands.end()) {
        return false;
    }
    
    auto& splitHands = playerSplitHands[playerIndex];
    
    if (handIndex >= static_cast<int>(splitHands.size())) {
        return false;
    }
    
    // Check if re-splitting is allowed
    if (!canPlayerResplit(playerIndex, handIndex)) {
        return false;
    }
    
    Player& player = players[playerIndex];
    SplitHands& handToSplit = splitHands[handIndex];
    
    std::cout << player.getName() << " re-splits hand " << (handIndex + 1) << "." << std::endl;
    
    // Get the two cards from the hand being split
    Card card1 = handToSplit.cards[0];
    Card card2 = handToSplit.cards[1];
    
    // Check if splitting Aces (special rules)
    bool splittingAces = (card1.getValue() == 1 && card2.getValue() == 1);
    
    // Create a new hand with the second card
    SplitHands newHand(splittingAces);
    newHand.addCard(card2);
    
    // Remove the second card from the original hand
    handToSplit.cards.pop_back();  // Remove the second card
    
    // Deal one new card to each hand
    handToSplit.addCard(deck.dealCard());
    newHand.addCard(deck.dealCard());
    
    // Insert the new hand right after the current hand to maintain order
    splitHands.insert(splitHands.begin() + handIndex + 1, newHand);
    
    std::cout << "Re-split successful! Now playing with " << splitHands.size() << " hands." << std::endl;
    
    // Display both hands after the split
    std::cout << "\nAfter re-split:" << std::endl;
    displaySplitHand(playerIndex, handIndex);
    displaySplitHand(playerIndex, handIndex + 1);
    
    return true;
}

bool SplitHand::playerDoublesDownSplit(int playerIndex, int handIndex) {
    auto& splitHands = playerSplitHands[playerIndex];
    if (handIndex >= static_cast<int>(splitHands.size())) {
        return false;
    }
    
    SplitHands& currentHand = splitHands[handIndex];
    
    if (!currentHand.canDouble()) {
        return false;
    }
    
    Card newCard = deck.dealCard();
    currentHand.addCard(newCard);
    currentHand.doubleDown();
    
    std::cout << "Doubled down on hand " << (handIndex + 1) 
              << " and drew: " << newCard.toString() << std::endl;
    
    displaySplitHand(playerIndex, handIndex);
    
    if (currentHand.isBusted()) {
        std::cout << "Hand " << (handIndex + 1) << " busts!" << std::endl;
    } else {
        std::cout << "Standing on hand " << (handIndex + 1) << std::endl;
    }
    
    return true;
}

void SplitHand::displaySplitHand(int playerIndex, int handIndex) const {
    const Player& player = players[playerIndex];
    const auto& splitHands = playerSplitHands.at(playerIndex);
    const SplitHands& currentHand = splitHands[handIndex];
    
    std::cout << player.getName() << "'s hand " << (handIndex + 1);
    
    if (currentHand.getBetMultiplier() > 1) {
        std::cout << " (DOUBLED)";
    }
    
    std::cout << ": ";
    
    for (size_t i = 0; i < currentHand.cards.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << currentHand.cards[i].toString();
    }
    
    std::cout << " (Total: " << currentHand.getTotalValue() << ")";
    
    if (currentHand.is21()) {
        std::cout << " - 21!";
    } else if (currentHand.isBusted()) {
        std::cout << " - BUST!";
    }
    
    if (currentHand.isAceSplit && currentHand.cards.size() == 2) {
        std::cout << " [Aces split - no more cards]";
    }
    
    std::cout << std::endl;
}

bool SplitHand::playerSplits(int playerIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
        return false;
    }

    Player& player = players[playerIndex];
    
    if (!canPlayerSplit(playerIndex)) {
        return false;
    }
    
    std::cout << player.getName() << " splits their pair." << std::endl;
    
    Card card1 = player.getCard(0);
    Card card2 = player.getCard(1);
    
    // Check if splitting Aces (special rules apply)
    bool splittingAces = (card1.getValue() == 1 && card2.getValue() == 1);
    
    if (splittingAces) {
        std::cout << "Splitting Aces - each hand gets only one additional card." << std::endl;
    }
    
    // Create split hands
    std::vector<SplitHands> splitHands(2, SplitHands(splittingAces));
    splitHands[0].addCard(card1);
    splitHands[1].addCard(card2);
    
    // Deal one card to each split hand
    splitHands[0].addCard(deck.dealCard());
    splitHands[1].addCard(deck.dealCard());
    
    // Store split hands
    playerSplitHands[playerIndex] = splitHands;
    
    playSplitHands(playerIndex);
    
    return true;
}