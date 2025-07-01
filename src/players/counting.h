#ifndef COUNTING_H
#define COUNTING_H

#include <string>
#include <map>
#include <random>
#include "card.h"
#include "deck.h"
#include "Stats.h"

enum class CountingSystem {
    HI_LO,      // +1 for 2-6, 0 for 7-9, -1 for 10-A
    KO,         // Knock Out system
    HI_OPT_I,   // Hi-Opt I system
    OMEGA_II    // Advanced counting system
};

struct CountingStats {
    int totalQuizzes;
    int correctRunningCount;
    int correctTrueCount;
    int totalRunningCountQuestions;
    int totalTrueCountQuestions;
    double averageResponseTime;
    
    CountingStats() : totalQuizzes(0), correctRunningCount(0), correctTrueCount(0),
                     totalRunningCountQuestions(0), totalTrueCountQuestions(0),
                     averageResponseTime(0.0) {}
};

class Counting {
private:
    CountingSystem currentSystem;
    int runningCount;
    bool countingEnabled;
    double quizProbability;  // Probability of quiz occurring each hand
    
    // Random number generation for quizzes
    std::mt19937 rng;
    std::uniform_real_distribution<double> quizChance;
    std::uniform_int_distribution<int> quizType;
    
    // Player statistics
    std::map<std::string, CountingStats> playerStats;
    
    // Deck reference for true count calculations
    const Deck* deck;
    
    // Card values for different counting systems
    std::map<CountingSystem, std::map<int, int>> countingValues;
    
    void initializeCountingSystems();
    int getCardCountValue(const Card& card, CountingSystem system) const;
    double calculateTrueCount() const;
    
    // Quiz management
    bool shouldTriggerQuiz();
    void askRunningCountQuiz(const std::string& playerName);
    void askTrueCountQuiz(const std::string& playerName);
    void recordQuizResult(const std::string& playerName, bool isRunningCount, 
                         bool correct, double responseTime);

public:
    Counting(const Deck* gameDeck);
    
    // Core counting functionality
    void updateCount(const Card& card);
    void resetCount();
    int getRunningCount() const;
    double getTrueCount() const;
    
    // System management
    void setCountingSystem(CountingSystem system);
    CountingSystem getCurrentSystem() const;
    std::string getSystemName(CountingSystem system) const;
    
    // Enable/disable features
    void enableCounting(bool enabled);
    void setQuizProbability(double probability); // 0.0 to 1.0
    bool isCountingEnabled() const;
    
    // Quiz system
    void triggerRandomQuiz(const std::string& playerName);
    void manualQuiz(const std::string& playerName, bool runningCountOnly = false);
    
    // Information display
    void displayCurrentCount() const;
    void displayCountingAdvice() const;
    void showCountingSystemInfo() const;
    
    // Statistics and analysis
    void displayPlayerCountingStats(const std::string& playerName) const;
    void displayAllCountingStats() const;
    void resetCountingStats();
    void resetCountingStats(const std::string& playerName);
    
    // Betting advice based on count
    std::string getBettingAdvice() const;
    int getSuggestedBetMultiplier() const; // 1-5x based on true count
    
    // Training mode
    void enterTrainingMode();
    void practiceCountingDrill(int numberOfCards = 20);
    
    // Accuracy tracking
    double getCountingAccuracy(const std::string& playerName) const;
    bool isPlayerCountingAccurately(const std::string& playerName, double threshold = 0.8) const;
    
    // Integration with game flow
    void processDealtCards(const std::vector<Card>& cards);
    bool checkForQuizOpportunity(const std::string& currentPlayer);

    int getDecksRemaining() const;
};

#endif