#include "Stats.h"
#include <iostream>
#include <iomanip>

Stats::Stats() : totalGamesPlayed(0) {}

void Stats::updatePlayerStats(const std::string& playerName, GameResult result) {
    switch (result) {
        case GameResult::WIN:
            playerWins[playerName]++;
            break;
        case GameResult::LOSS:
            playerLosses[playerName]++;
            break;
        case GameResult::PUSH:
            playerPushes[playerName]++;
            break;
        case GameResult::BLACKJACK:
            playerBlackjacks[playerName]++;
            playerWins[playerName]++;  // Blackjack also counts as a win
            break;
    }
}

void Stats::incrementGamesPlayed() {
    totalGamesPlayed++;
}

void Stats::displayFinalResults(const Counting* countingSystem, 
    const BasicStrat* basicStrategy) const {
std::cout << "\n======================================" << std::endl;
std::cout << "           FINAL GAME RESULTS            " << std::endl;
std::cout << "======================================" << std::endl;

if (totalGamesPlayed == 0) {
std::cout << "No games played yet!" << std::endl;
std::cout << "======================================" << std::endl;
return;
}

std::cout << "Total Games Played: " << totalGamesPlayed << std::endl;
std::cout << "======================================" << std::endl;

// Display stats for each player who has played
for (const auto& pair : playerWins) {
const std::string& name = pair.first;
displayPlayerStats(name);
}

// Overall session statistics
int totalWins = 0, totalLosses = 0, totalPushes = 0, totalBlackjacks = 0;
for (const auto& pair : playerWins) {
totalWins += pair.second;
}
for (const auto& pair : playerLosses) {
totalLosses += pair.second;
}
for (const auto& pair : playerPushes) {
totalPushes += pair.second;
}
for (const auto& pair : playerBlackjacks) {
totalBlackjacks += pair.second;
}

int totalHands = totalWins + totalLosses + totalPushes;

std::cout << "\nSESSION TOTALS:" << std::endl;
std::cout << "  Total Hands Played: " << totalHands << std::endl;
std::cout << "  Total Wins: " << totalWins << std::endl;
std::cout << "  Total Losses: " << totalLosses << std::endl;
std::cout << "  Total Pushes: " << totalPushes << std::endl;
std::cout << "  Total Blackjacks: " << totalBlackjacks << std::endl;

if (totalHands > 0) {
double overallWinRate = (static_cast<double>(totalWins) / totalHands) * 100.0;
std::cout << "  Overall Win Rate: " << std::fixed << std::setprecision(1) 
<< overallWinRate << "%" << std::endl;
}

// CARD COUNTING INFORMATION
if (countingSystem && countingSystem->isCountingEnabled()) {
std::cout << "\n======== CARD COUNTING STATUS ========" << std::endl;
std::cout << "Counting System: " << countingSystem->getSystemName(countingSystem->getCurrentSystem()) << std::endl;
std::cout << "Final Running Count: " << countingSystem->getRunningCount() << std::endl;
std::cout << "Final True Count: " << std::fixed << std::setprecision(2) 
<< countingSystem->getTrueCount() << std::endl;
std::cout << "Decks Remaining: " << countingSystem->getDecksRemaining() << std::endl;
std::cout << "Final Betting Advice: " << countingSystem->getBettingAdvice() << std::endl;

// Display counting accuracy for each player
std::cout << "\nCOUNTING ACCURACY:" << std::endl;
for (const auto& pair : playerWins) {
const std::string& playerName = pair.first;
double accuracy = countingSystem->getCountingAccuracy(playerName);
if (accuracy > 0) {
std::cout << "  " << playerName << ": " << std::fixed << std::setprecision(1) 
<< accuracy << "% accuracy" << std::endl;
} else {
std::cout << "  " << playerName << ": No counting data" << std::endl;
}
}
std::cout << "======================================" << std::endl;
} else {
std::cout << "\nCard counting was disabled for this session." << std::endl;
}

// BASIC STRATEGY INFORMATION
if (basicStrategy) {
std::cout << "\n======== BASIC STRATEGY ANALYSIS ========" << std::endl;

bool hasStrategyData = false;
for (const auto& pair : playerWins) {
const std::string& playerName = pair.first;
double accuracy = basicStrategy->getAccuracyRate(playerName);
if (accuracy > 0) {
hasStrategyData = true;
int deviations = basicStrategy->getDeviationCount(playerName);
std::cout << playerName << ":" << std::endl;
std::cout << "  Strategy Accuracy: " << std::fixed << std::setprecision(1) 
<< accuracy << "%" << std::endl;
std::cout << "  Strategy Deviations: " << deviations << std::endl;

// Rating based on accuracy
if (accuracy >= 95.0) {
std::cout << "  Rating: EXPERT - Excellent strategy play!" << std::endl;
} else if (accuracy >= 90.0) {
std::cout << "  Rating: ADVANCED - Very good strategy!" << std::endl;
} else if (accuracy >= 80.0) {
std::cout << "  Rating: INTERMEDIATE - Good with room for improvement" << std::endl;
} else if (accuracy >= 70.0) {
std::cout << "  Rating: BEGINNER - Keep practicing!" << std::endl;
} else {
std::cout << "  Rating: NEEDS WORK - Study basic strategy more" << std::endl;
}
std::cout << "  ------------------" << std::endl;
}
}

if (!hasStrategyData) {
std::cout << "No basic strategy data recorded this session." << std::endl;
}
std::cout << "=========================================" << std::endl;
}

std::cout << "\nThanks for playing! Come back soon!" << std::endl;
std::cout << "======================================" << std::endl;
}

void Stats::displayQuickStats() const {
    if (totalGamesPlayed == 0) {
        std::cout << "No games played yet!" << std::endl;
        return;
    }
    
    std::cout << "\n=== QUICK STATS ===" << std::endl;
    std::cout << "Games Played: " << totalGamesPlayed << std::endl;
    
    // Quick summary of all players
    int totalWins = 0, totalLosses = 0, totalPushes = 0;
    for (const auto& pair : playerWins) {
        totalWins += pair.second;
    }
    for (const auto& pair : playerLosses) {
        totalLosses += pair.second;
    }
    for (const auto& pair : playerPushes) {
        totalPushes += pair.second;
    }
    
    int totalHands = totalWins + totalLosses + totalPushes;
    if (totalHands > 0) {
        double winRate = (static_cast<double>(totalWins) / totalHands) * 100.0;
        std::cout << "Overall Win Rate: " << std::fixed << std::setprecision(1) 
                  << winRate << "%" << std::endl;
    }
    std::cout << "===================" << std::endl;
}

void Stats::resetStats() {
    playerWins.clear();
    playerLosses.clear();
    playerPushes.clear();
    playerBlackjacks.clear();
    totalGamesPlayed = 0;
    std::cout << "Game statistics have been reset!" << std::endl;
}

void Stats::displayPlayerStats(const std::string& playerName) const {
    if (!hasPlayerData(playerName)) {
        std::cout << "No statistics found for player: " << playerName << std::endl;
        return;
    }
    
    int wins = getPlayerWins(playerName);
    int losses = getPlayerLosses(playerName);
    int pushes = getPlayerPushes(playerName);
    int blackjacks = getPlayerBlackjacks(playerName);
    int totalHands = getPlayerTotalHands(playerName);
    
    std::cout << "\n" << playerName << ":" << std::endl;
    std::cout << "  Wins: " << wins;
    if (blackjacks > 0) {
        std::cout << " (including " << blackjacks << " blackjack" 
                  << (blackjacks == 1 ? "" : "s") << ")";
    }
    std::cout << std::endl;
    std::cout << "  Losses: " << losses << std::endl;
    std::cout << "  Pushes: " << pushes << std::endl;
    std::cout << "  Total Hands: " << totalHands << std::endl;
    
    if (totalHands > 0) {
        double winRate = getPlayerWinRate(playerName);
        std::cout << "  Win Rate: " << std::fixed << std::setprecision(1) 
                  << winRate << "%" << std::endl;
        
        if (blackjacks > 0) {
            double blackjackRate = (static_cast<double>(blackjacks) / totalHands) * 100.0;
            std::cout << "  Blackjack Rate: " << std::fixed << std::setprecision(1) 
                      << blackjackRate << "%" << std::endl;
        }
    }
    std::cout << "  ------------------" << std::endl;
}

// Getter methods
int Stats::getTotalGamesPlayed() const {
    return totalGamesPlayed;
}

int Stats::getPlayerWins(const std::string& playerName) const {
    auto it = playerWins.find(playerName);
    return (it != playerWins.end()) ? it->second : 0;
}

int Stats::getPlayerLosses(const std::string& playerName) const {
    auto it = playerLosses.find(playerName);
    return (it != playerLosses.end()) ? it->second : 0;
}

int Stats::getPlayerPushes(const std::string& playerName) const {
    auto it = playerPushes.find(playerName);
    return (it != playerPushes.end()) ? it->second : 0;
}

int Stats::getPlayerBlackjacks(const std::string& playerName) const {
    auto it = playerBlackjacks.find(playerName);
    return (it != playerBlackjacks.end()) ? it->second : 0;
}

double Stats::getPlayerWinRate(const std::string& playerName) const {
    int totalHands = getPlayerTotalHands(playerName);
    if (totalHands == 0) return 0.0;
    
    int wins = getPlayerWins(playerName);
    return (static_cast<double>(wins) / totalHands) * 100.0;
}

std::vector<std::string> Stats::getAllPlayerNames() const {
    std::vector<std::string> names;
    for (const auto& pair : playerWins) {
        names.push_back(pair.first);
    }
    return names;
}

bool Stats::hasPlayerData(const std::string& playerName) const {
    return playerWins.find(playerName) != playerWins.end();
}

int Stats::getPlayerTotalHands(const std::string& playerName) const {
    return getPlayerWins(playerName) + getPlayerLosses(playerName) + getPlayerPushes(playerName);
}