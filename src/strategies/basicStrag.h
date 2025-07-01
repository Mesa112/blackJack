#ifndef BASICSTRAT_H
#define BASICSTRAT_H

#include <string>
#include <map>

class Player;
class Dealer;

enum class Action {
    HIT,
    STAND,
    DOUBLE,
    SPLIT,
    SURRENDER
};

enum class HandType {
    HARD,    
    SOFT,    
    PAIR     
};
class BasicStrat {
private:
    // Strategy tables for different hand types
    std::map<std::pair<int, int>, Action> hardTotalsTable;
    std::map<std::pair<int, int>, Action> softTotalsTable;
    std::map<std::pair<int, int>, Action> pairsTable;
    
    // Statistics tracking
    std::map<std::string, int> correctActions;
    std::map<std::string, int> totalActions;
    std::map<std::string, int> deviations;
    
    void initializeStrategyTables();
    HandType determineHandType(const Player& player) const;
    int getHandValue(const Player& player, HandType& handType) const;
    int getDealerUpValue(const Dealer& dealer) const;
    
public:
    BasicStrat();
    
    // Strategy lookup
    Action getOptimalAction(const Player& player, const Dealer& dealer, 
                           bool canDouble = true, bool canSurrender = true) const;
    
    // Player guidance
    std::string getActionString(Action action) const;
    void suggestAction(const Player& player, const Dealer& dealer,
                      bool canDouble = true, bool canSurrender = true) const;
    
    // Statistics tracking
    void recordPlayerAction(const std::string& playerName, const Player& player, 
                           const Dealer& dealer, Action takenAction,
                           bool canDouble = true, bool canSurrender = true);
    
    // Display methods
    void displayStrategyTable(HandType handType) const;
    void displayPlayerStats(const std::string& playerName) const;
    void displayAllStats() const;
    void resetStats();
    
    // Analysis methods
    double getAccuracyRate(const std::string& playerName) const;
    int getDeviationCount(const std::string& playerName) const;
    bool isPlayerFollowingStrategy(const std::string& playerName, double threshold = 0.8) const;
    
    // Configuration
    void enableStrategyMode(bool enabled);
    void setDifficultyLevel(int level); // 1=Basic, 2=Advanced with surrender, 3=Expert
};

#endif