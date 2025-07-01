#ifndef SPLITHAND_H
#define SPLITHAND_H

#include <iostream>
#include <map>
#include <vector>
#include "card.h"
#include "player.h"

class Deck;

class SplitHand {
private:
    struct SplitHands {
        std::vector<Card> cards;
        bool isActive;
        bool canDoubleDown;   
        bool isAceSplit;      
        int betMultiplier;      

        SplitHands(bool acesSplit = false) 
            : isActive(true), canDoubleDown(true), isAceSplit(acesSplit), betMultiplier(1) {}
        
        void addCard(const Card& card) {
            cards.push_back(card);
            // after you split aces you cant hit again
            if (isAceSplit && cards.size() >= 2) {
                isActive = false;
            }
            // only double on the firsthit
            if (cards.size() > 2) {
                canDoubleDown = false;
            }
        }
        
        int getTotalValue() const {
            int totalVal = 0;
            int aceCount = 0;
            for (const auto& card : cards) {
                int cardValue = card.getValue();
                if (cardValue == 1) {
                    aceCount++;
                    totalVal += 11;
                } else if (cardValue > 10) {
                    totalVal += 10;
                } else {
                    totalVal += cardValue;
                }
            }
            while(totalVal > 21 && aceCount > 0) {
                totalVal -= 10;
                aceCount--;
            }
            return totalVal;
        }
        
        bool isBusted() const {
            return getTotalValue() > 21;
        }
        
        bool isBlackjack() const {
            
            return false;  // in real life you can not get blackjack from splitting cards
        }
        
        bool is21() const {
            
            return getTotalValue() == 21;
        }
        
        size_t getCardCount() const {
            return cards.size();
        }
        
        bool canDouble() const {
            return canDoubleDown && cards.size() == 2 && !isBusted();
        }
        
        void doubleDown() {
            betMultiplier = 2;
            canDoubleDown = false;
        }
        
        int getBetMultiplier() const {
            return betMultiplier;
        }
    };

    std::map<int, std::vector<SplitHands>> playerSplitHands;
    std::vector<Player>& players;
    Deck& deck;
    int maxSplits; 

public:
    SplitHand(std::vector<Player>& gamePlayers, Deck& gameDeck, int maxSplitsAllowed = 4);
    
    bool playerSplits(int playerIndex);
    void playSplitHands(int playerIndex);
    void playSingleSplitHand(int playerIndex, int handIndex);
    void displaySplitHand(int playerIndex, int handIndex) const;
    bool canPlayerSplit(int playerIndex);
    bool canPlayerResplit(int playerIndex, int handIndex);  
    bool playerDoublesDownSplit(int playerIndex, int handIndex);  
    bool reSplit(int playerIndex, int handIndex);
    
    const std::map<int, std::vector<SplitHands>>& getPlayerSplitHands() const {
        return playerSplitHands;
    }
};

#endif