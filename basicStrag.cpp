#include "basicStrag.h"

#include <iostream>
#include <iomanip>

#include "player.h"
#include "Dealer.h"


BasicStrat::BasicStrat() {
    initializeStrategyTables();
}

void BasicStrat::initializeStrategyTables() {
    // Hard totals strategy table (player total, dealer up card) -> action
    // Dealer up cards: A=1, 2-9=face value, T/J/Q/K=10
    
    // Hard totals (no aces counted as 11)
    // 8 and below: always hit
    for (int total = 5; total <= 8; ++total) {
        for (int dealer = 1; dealer <= 10; ++dealer) {
            hardTotalsTable[{total, dealer}] = Action::HIT;
        }
    }
    
    // 9: Double vs 3-6, otherwise hit
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 3 && dealer <= 6) {
            hardTotalsTable[{9, dealer}] = Action::DOUBLE;
        } else {
            hardTotalsTable[{9, dealer}] = Action::HIT;
        }
    }
    
    // 10: Double vs 2-9, otherwise hit
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 2 && dealer <= 9) {
            hardTotalsTable[{10, dealer}] = Action::DOUBLE;
        } else {
            hardTotalsTable[{10, dealer}] = Action::HIT;
        }
    }
    
    // 11: Double vs 2-10, hit vs ace
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer == 1) {
            hardTotalsTable[{11, dealer}] = Action::HIT;
        } else {
            hardTotalsTable[{11, dealer}] = Action::DOUBLE;
        }
    }
    
    // 12: Stand vs 4-6, otherwise hit
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 4 && dealer <= 6) {
            hardTotalsTable[{12, dealer}] = Action::STAND;
        } else {
            hardTotalsTable[{12, dealer}] = Action::HIT;
        }
    }
    
    // 13-16: Stand vs 2-6, otherwise hit
    for (int total = 13; total <= 16; ++total) {
        for (int dealer = 1; dealer <= 10; ++dealer) {
            if (dealer >= 2 && dealer <= 6) {
                hardTotalsTable[{total, dealer}] = Action::STAND;
            } else {
                hardTotalsTable[{total, dealer}] = Action::HIT;
            }
        }
    }
    
    // 17 and above: always stand
    for (int total = 17; total <= 21; ++total) {
        for (int dealer = 1; dealer <= 10; ++dealer) {
            hardTotalsTable[{total, dealer}] = Action::STAND;
        }
    }
    
    // Soft totals strategy table (ace counted as 11)
    // A,2 and A,3: Double vs 5-6, otherwise hit
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 5 && dealer <= 6) {
            softTotalsTable[{13, dealer}] = Action::DOUBLE; // A,2
            softTotalsTable[{14, dealer}] = Action::DOUBLE; // A,3
        } else {
            softTotalsTable[{13, dealer}] = Action::HIT;
            softTotalsTable[{14, dealer}] = Action::HIT;
        }
    }
    
    // A,4 and A,5: Double vs 4-6, otherwise hit
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 4 && dealer <= 6) {
            softTotalsTable[{15, dealer}] = Action::DOUBLE; // A,4
            softTotalsTable[{16, dealer}] = Action::DOUBLE; // A,5
        } else {
            softTotalsTable[{15, dealer}] = Action::HIT;
            softTotalsTable[{16, dealer}] = Action::HIT;
        }
    }
    
    // A,6: Double vs 3-6, otherwise hit
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 3 && dealer <= 6) {
            softTotalsTable[{17, dealer}] = Action::DOUBLE; // A,6
        } else {
            softTotalsTable[{17, dealer}] = Action::HIT;
        }
    }
    
    // A,7: Stand vs 2,7,8; Double vs 3-6; Hit vs 9,10,A
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer == 2 || dealer == 7 || dealer == 8) {
            softTotalsTable[{18, dealer}] = Action::STAND; // A,7
        } else if (dealer >= 3 && dealer <= 6) {
            softTotalsTable[{18, dealer}] = Action::DOUBLE;
        } else {
            softTotalsTable[{18, dealer}] = Action::HIT;
        }
    }
    
    // A,8 and A,9: Always stand
    for (int dealer = 1; dealer <= 10; ++dealer) {
        softTotalsTable[{19, dealer}] = Action::STAND; // A,8
        softTotalsTable[{20, dealer}] = Action::STAND; // A,9
    }
    
    // Pairs strategy table
    // A,A: Always split
    for (int dealer = 1; dealer <= 10; ++dealer) {
        pairsTable[{1, dealer}] = Action::SPLIT;
    }
    
    // 2,2 and 3,3: Split vs 2-7, otherwise hit
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 2 && dealer <= 7) {
            pairsTable[{2, dealer}] = Action::SPLIT;
            pairsTable[{3, dealer}] = Action::SPLIT;
        } else {
            pairsTable[{2, dealer}] = Action::HIT;
            pairsTable[{3, dealer}] = Action::HIT;
        }
    }
    
    // 4,4: Hit vs all (some variations split vs 5-6)
    for (int dealer = 1; dealer <= 10; ++dealer) {
        pairsTable[{4, dealer}] = Action::HIT;
    }
    
    // 5,5: Never split, treat as hard 10
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 2 && dealer <= 9) {
            pairsTable[{5, dealer}] = Action::DOUBLE;
        } else {
            pairsTable[{5, dealer}] = Action::HIT;
        }
    }
    
    // 6,6: Split vs 2-6, otherwise hit
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 2 && dealer <= 6) {
            pairsTable[{6, dealer}] = Action::SPLIT;
        } else {
            pairsTable[{6, dealer}] = Action::HIT;
        }
    }
    
    // 7,7: Split vs 2-7, otherwise hit
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer >= 2 && dealer <= 7) {
            pairsTable[{7, dealer}] = Action::SPLIT;
        } else {
            pairsTable[{7, dealer}] = Action::HIT;
        }
    }
    
    // 8,8: Always split
    for (int dealer = 1; dealer <= 10; ++dealer) {
        pairsTable[{8, dealer}] = Action::SPLIT;
    }
    
    // 9,9: Split vs 2-9 except 7, stand vs 7,10,A
    for (int dealer = 1; dealer <= 10; ++dealer) {
        if (dealer == 7 || dealer == 10 || dealer == 1) {
            pairsTable[{9, dealer}] = Action::STAND;
        } else {
            pairsTable[{9, dealer}] = Action::SPLIT;
        }
    }
    
    // 10,10: Never split, always stand
    for (int dealer = 1; dealer <= 10; ++dealer) {
        pairsTable[{10, dealer}] = Action::STAND;
    }
}

HandType BasicStrat::determineHandType(const Player& player) const {
    if (player.getCardCount() != 2) {
        // For hands with more than 2 cards, check if we have a soft total
        int aces = 0;
        int total = 0;
        
        for (size_t i = 0; i < player.getCardCount(); ++i) {
            int value = player.getCard(i).getValue();
            if (value == 1) aces++;
            else if (value > 10) total += 10;
            else total += value;
        }
        
        // Check if we can count an ace as 11 without busting
        if (aces > 0 && (total + 11 + (aces - 1)) <= 21) {
            return HandType::SOFT;
        }
        return HandType::HARD;
    }
    
    // For 2-card hands
    Card card1 = player.getCard(0);
    Card card2 = player.getCard(1);
    int value1 = card1.getValue();
    int value2 = card2.getValue();
    
    // Check for pairs first
    if (value1 == value2) {
        return HandType::PAIR;
    }
    
    // Check for face card pairs (both worth 10)
    bool card1IsFace = (value1 >= 10);
    bool card2IsFace = (value2 >= 10);
    if (card1IsFace && card2IsFace) {
        return HandType::PAIR;
    }
    
    // Check for soft totals (ace + another card)
    if (value1 == 1 || value2 == 1) {
        return HandType::SOFT;
    }
    
    return HandType::HARD;
}

int BasicStrat::getHandValue(const Player& player, HandType& handType) const {
    if (handType == HandType::PAIR) {
        return player.getCard(0).getValue() > 10 ? 10 : player.getCard(0).getValue();
    }
    
    return player.getTotalValue();
}

int BasicStrat::getDealerUpValue(const Dealer& dealer) const {
    if (dealer.getCardCount() == 0) return 0;
    
    int value = dealer.getCard(0).getValue(); // Up card
    if (value > 10) return 10; // Face cards
    return value; // Ace = 1, others face value
}

Action BasicStrat::getOptimalAction(const Player& player, const Dealer& dealer, 
                                   bool canDouble, bool canSurrender) const {
    HandType handType = determineHandType(player);
    int handValue = getHandValue(player, handType);
    int dealerUp = getDealerUpValue(dealer);
    
    Action action;
    
    switch (handType) {
        case HandType::PAIR: {
            auto it = pairsTable.find({handValue, dealerUp});
            action = (it != pairsTable.end()) ? it->second : Action::HIT;
            break;
        }
        case HandType::SOFT: {
            auto it = softTotalsTable.find({handValue, dealerUp});
            action = (it != softTotalsTable.end()) ? it->second : Action::STAND;
            break;
        }
        case HandType::HARD: {
            auto it = hardTotalsTable.find({handValue, dealerUp});
            action = (it != hardTotalsTable.end()) ? it->second : Action::STAND;
            break;
        }
    }
    
    // Adjust for game constraints
    if (action == Action::DOUBLE && !canDouble) {
        action = Action::HIT;
    }
    if (action == Action::SURRENDER && !canSurrender) {
        // Fall back to basic strategy without surrender
        if (handValue >= 15 && handValue <= 16 && (dealerUp == 9 || dealerUp == 10 || dealerUp == 1)) {
            action = Action::HIT;
        } else {
            action = Action::STAND;
        }
    }
    
    return action;
}

std::string BasicStrat::getActionString(Action action) const {
    switch (action) {
        case Action::HIT: return "Hit";
        case Action::STAND: return "Stand";
        case Action::DOUBLE: return "Double Down";
        case Action::SPLIT: return "Split";
        case Action::SURRENDER: return "Surrender";
        default: return "Unknown";
    }
}

void BasicStrat::suggestAction(const Player& player, const Dealer& dealer,
                              bool canDouble, bool canSurrender) const {
    Action optimal = getOptimalAction(player, dealer, canDouble, canSurrender);
    HandType handType = determineHandType(player);
    
    std::cout << "\n--- BASIC STRATEGY SUGGESTION ---" << std::endl;
    std::cout << "Your hand: " << player.getTotalValue();
    std::cout << " (" << (handType == HandType::SOFT ? "Soft" : 
                         handType == HandType::PAIR ? "Pair" : "Hard") << ")" << std::endl;
    std::cout << "Dealer shows: " << getDealerUpValue(dealer) << std::endl;
    std::cout << "Optimal action: " << getActionString(optimal) << std::endl;
    std::cout << "--------------------------------" << std::endl;
}

void BasicStrat::recordPlayerAction(const std::string& playerName, const Player& player, 
                                   const Dealer& dealer, Action takenAction,
                                   bool canDouble, bool canSurrender) {
    Action optimalAction = getOptimalAction(player, dealer, canDouble, canSurrender);
    
    totalActions[playerName]++;
    
    if (takenAction == optimalAction) {
        correctActions[playerName]++;
    } else {
        deviations[playerName]++;
    }
}

void BasicStrat::displayPlayerStats(const std::string& playerName) const {
    auto totalIt = totalActions.find(playerName);
    if (totalIt == totalActions.end() || totalIt->second == 0) {
        std::cout << "No strategy data for " << playerName << std::endl;
        return;
    }
    
    int total = totalIt->second;
    int correct = (correctActions.find(playerName) != correctActions.end()) ? 
                  correctActions.at(playerName) : 0;
    int incorrect = total - correct;
    double accuracy = (static_cast<double>(correct) / total) * 100.0;
    
    std::cout << "\n=== BASIC STRATEGY STATS: " << playerName << " ===" << std::endl;
    std::cout << "Total Actions: " << total << std::endl;
    std::cout << "Correct Actions: " << correct << std::endl;
    std::cout << "Incorrect Actions: " << incorrect << std::endl;
    std::cout << "Accuracy Rate: " << std::fixed << std::setprecision(1) 
              << accuracy << "%" << std::endl;
    
    if (accuracy >= 90.0) {
        std::cout << "Rating: EXCELLENT! You're following basic strategy very well!" << std::endl;
    } else if (accuracy >= 80.0) {
        std::cout << "Rating: GOOD! Minor deviations from optimal play." << std::endl;
    } else if (accuracy >= 70.0) {
        std::cout << "Rating: FAIR. Room for improvement in strategy." << std::endl;
    } else {
        std::cout << "Rating: NEEDS WORK. Consider studying basic strategy more." << std::endl;
    }
    std::cout << "=============================================" << std::endl;
}

void BasicStrat::displayAllStats() const {
    std::cout << "\n======== BASIC STRATEGY OVERVIEW ========" << std::endl;
    
    if (totalActions.empty()) {
        std::cout << "No strategy data recorded yet." << std::endl;
        std::cout << "=========================================" << std::endl;
        return;
    }
    
    for (const auto& pair : totalActions) {
        displayPlayerStats(pair.first);
    }
}

void BasicStrat::resetStats() {
    correctActions.clear();
    totalActions.clear();
    deviations.clear();
    std::cout << "Basic strategy statistics have been reset!" << std::endl;
}

double BasicStrat::getAccuracyRate(const std::string& playerName) const {
    auto totalIt = totalActions.find(playerName);
    if (totalIt == totalActions.end() || totalIt->second == 0) {
        return 0.0;
    }
    
    int total = totalIt->second;
    int correct = (correctActions.find(playerName) != correctActions.end()) ? 
                  correctActions.at(playerName) : 0;
    
    return (static_cast<double>(correct) / total) * 100.0;
}

int BasicStrat::getDeviationCount(const std::string& playerName) const {
    auto it = deviations.find(playerName);
    return (it != deviations.end()) ? it->second : 0;
}

bool BasicStrat::isPlayerFollowingStrategy(const std::string& playerName, double threshold) const {
    return getAccuracyRate(playerName) >= threshold;
}