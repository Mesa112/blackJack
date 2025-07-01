#ifndef STATS_H
#define STATS_H

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "counting.h"
#include "basicStrag.h"

class Counting;
class BasicStrat;


enum class GameResult {
    WIN,
    LOSS,
    PUSH,
    BLACKJACK
};

class Stats {
private:
    std::map<std::string, int> playerWins;
    std::map<std::string, int> playerLosses;
    std::map<std::string, int> playerPushes;
    std::map<std::string, int> playerBlackjacks;
    int totalGamesPlayed;

public:
    Stats();
    
    // Core functionality
    void updatePlayerStats(const std::string& playerName, GameResult result);
    void incrementGamesPlayed();
    
    // Display methods
    void displayFinalResults(const Counting* countingSystem = nullptr, 
        const BasicStrat* basicStrategy = nullptr) const;
    void displayQuickStats() const;
    void displayPlayerStats(const std::string& playerName) const;
    void resetStats();
    
    // Getter methods
    int getTotalGamesPlayed() const;
    int getPlayerWins(const std::string& playerName) const;
    int getPlayerLosses(const std::string& playerName) const;
    int getPlayerPushes(const std::string& playerName) const;
    int getPlayerBlackjacks(const std::string& playerName) const;
    double getPlayerWinRate(const std::string& playerName) const;
    std::vector<std::string> getAllPlayerNames() const;
    
    // Utility methods
    bool hasPlayerData(const std::string& playerName) const;
    int getPlayerTotalHands(const std::string& playerName) const;
};

#endif