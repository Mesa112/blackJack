#include "counting.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>

Counting::Counting(const Deck* gameDeck) 
    : currentSystem(CountingSystem::HI_LO), runningCount(0), countingEnabled(false),
      quizProbability(0.15), rng(std::random_device{}()), 
      quizChance(0.0, 1.0), quizType(1, 2), deck(gameDeck) {
    initializeCountingSystems();
}

void Counting::initializeCountingSystems() {
    // Hi-Lo System: +1 for 2-6, 0 for 7-9, -1 for 10-A
    countingValues[CountingSystem::HI_LO] = {
        {1, -1},  // Ace
        {2, +1}, {3, +1}, {4, +1}, {5, +1}, {6, +1},  // 2-6
        {7, 0}, {8, 0}, {9, 0},                        // 7-9
        {10, -1}, {11, -1}, {12, -1}, {13, -1}         // 10, J, Q, K
    };
    
    // KO (Knock Out) System: +1 for 2-7, 0 for 8-9, -1 for 10-A
    countingValues[CountingSystem::KO] = {
        {1, -1},  // Ace
        {2, +1}, {3, +1}, {4, +1}, {5, +1}, {6, +1}, {7, +1},  // 2-7
        {8, 0}, {9, 0},                                         // 8-9
        {10, -1}, {11, -1}, {12, -1}, {13, -1}                  // 10, J, Q, K
    };
    
    // Hi-Opt I System: +1 for 3-6, 0 for 2,7-9,A, -1 for 10-K
    countingValues[CountingSystem::HI_OPT_I] = {
        {1, 0},   // Ace
        {2, 0},   // 2
        {3, +1}, {4, +1}, {5, +1}, {6, +1},            // 3-6
        {7, 0}, {8, 0}, {9, 0},                        // 7-9
        {10, -1}, {11, -1}, {12, -1}, {13, -1}         // 10, J, Q, K
    };
    
    // Omega II System (advanced): Various values
    countingValues[CountingSystem::OMEGA_II] = {
        {1, 0},   // Ace
        {2, +1},  // 2
        {3, +1}, {4, +2}, {5, +2}, {6, +2},            // 3-6
        {7, +1},  // 7
        {8, 0}, {9, -1},                               // 8-9
        {10, -2}, {11, -2}, {12, -2}, {13, -2}         // 10, J, Q, K
    };
}

int Counting::getCardCountValue(const Card& card, CountingSystem system) const {
    int value = card.getValue();
    auto systemIt = countingValues.find(system);
    if (systemIt == countingValues.end()) return 0;
    
    auto valueIt = systemIt->second.find(value);
    return (valueIt != systemIt->second.end()) ? valueIt->second : 0;
}

void Counting::updateCount(const Card& card) {
    if (!countingEnabled) return;
    
    int cardValue = getCardCountValue(card, currentSystem);
    runningCount += cardValue;
}

void Counting::resetCount() {
    runningCount = 0;
    std::cout << "Running count reset to 0." << std::endl;
}

int Counting::getRunningCount() const {
    return runningCount;
}

double Counting::calculateTrueCount() const {
    if (!deck) return 0.0;
    
    int decksRemaining = getDecksRemaining();
    if (decksRemaining <= 0) return 0.0;
    
    return static_cast<double>(runningCount) / decksRemaining;
}

double Counting::getTrueCount() const {
    return calculateTrueCount();
}

int Counting::getDecksRemaining() const {
    if (!deck) return 1;
    
    int cardsRemaining = deck->getCardsRemaining();
    return std::max(1, (cardsRemaining + 51) / 52); // Round up to nearest deck
}

void Counting::setCountingSystem(CountingSystem system) {
    currentSystem = system;
    resetCount(); // Reset count when changing systems
    std::cout << "Counting system changed to: " << getSystemName(system) << std::endl;
}

CountingSystem Counting::getCurrentSystem() const {
    return currentSystem;
}

std::string Counting::getSystemName(CountingSystem system) const {
    switch (system) {
        case CountingSystem::HI_LO: return "Hi-Lo";
        case CountingSystem::KO: return "Knock Out (KO)";
        case CountingSystem::HI_OPT_I: return "Hi-Opt I";
        case CountingSystem::OMEGA_II: return "Omega II";
        default: return "Unknown";
    }
}

void Counting::enableCounting(bool enabled) {
    countingEnabled = enabled;
    if (enabled) {
        std::cout << "Card counting enabled with " << getSystemName(currentSystem) << " system." << std::endl;
        std::cout << "Quiz probability set to " << (quizProbability * 100) << "%." << std::endl;
    } else {
        std::cout << "Card counting disabled." << std::endl;
    }
}

void Counting::setQuizProbability(double probability) {
    quizProbability = std::max(0.0, std::min(1.0, probability));
    std::cout << "Quiz probability set to " << (quizProbability * 100) << "%." << std::endl;
}

bool Counting::isCountingEnabled() const {
    return countingEnabled;
}

bool Counting::shouldTriggerQuiz() {
    return countingEnabled && (quizChance(rng) < quizProbability);
}

void Counting::askRunningCountQuiz(const std::string& playerName) {
    auto start = std::chrono::steady_clock::now();
    
    std::cout << "\n*** COUNTING QUIZ ***" << std::endl;
    std::cout << "What is the current running count?" << std::endl;
    std::cout << "Your answer: ";
    
    int playerAnswer;
    std::cin >> playerAnswer;
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    double responseTime = duration.count() / 1000.0;
    
    bool correct = (playerAnswer == runningCount);
    
    if (correct) {
        std::cout << "Correct! The running count is " << runningCount << std::endl;
    } else {
        std::cout << "Incorrect. The running count is " << runningCount 
                  << " (you said " << playerAnswer << ")" << std::endl;
    }
    
    recordQuizResult(playerName, true, correct, responseTime);
}

void Counting::askTrueCountQuiz(const std::string& playerName) {
    auto start = std::chrono::steady_clock::now();
    
    double actualTrueCount = getTrueCount();
    
    std::cout << "\n*** COUNTING QUIZ ***" << std::endl;
    std::cout << "What is the current true count? (to nearest 0.5)" << std::endl;
    std::cout << "Running count: " << runningCount << std::endl;
    std::cout << "Decks remaining: " << getDecksRemaining() << std::endl;
    std::cout << "Your answer: ";
    
    double playerAnswer;
    std::cin >> playerAnswer;
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    double responseTime = duration.count() / 1000.0;
    
    // Allow for rounding tolerance (±0.5)
    bool correct = std::abs(playerAnswer - actualTrueCount) <= 0.5;
    
    if (correct) {
        std::cout << "Correct! The true count is approximately " 
                  << std::fixed << std::setprecision(1) << actualTrueCount << std::endl;
    } else {
        std::cout << "Incorrect. The true count is approximately " 
                  << std::fixed << std::setprecision(1) << actualTrueCount 
                  << " (you said " << playerAnswer << ")" << std::endl;
    }
    
    recordQuizResult(playerName, false, correct, responseTime);
}

void Counting::recordQuizResult(const std::string& playerName, bool isRunningCount, 
                               bool correct, double responseTime) {
    CountingStats& stats = playerStats[playerName];
    stats.totalQuizzes++;
    
    if (isRunningCount) {
        stats.totalRunningCountQuestions++;
        if (correct) stats.correctRunningCount++;
    } else {
        stats.totalTrueCountQuestions++;
        if (correct) stats.correctTrueCount++;
    }
    
    // Update average response time
    stats.averageResponseTime = ((stats.averageResponseTime * (stats.totalQuizzes - 1)) + responseTime) 
                               / stats.totalQuizzes;
}

void Counting::triggerRandomQuiz(const std::string& playerName) {
    if (!shouldTriggerQuiz()) return;
    
    // Randomly choose between running count and true count quiz
    int quiz = quizType(rng);
    
    if (quiz == 1) {
        askRunningCountQuiz(playerName);
    } else {
        askTrueCountQuiz(playerName);
    }
}

void Counting::manualQuiz(const std::string& playerName, bool runningCountOnly) {
    if (runningCountOnly) {
        askRunningCountQuiz(playerName);
    } else {
        std::cout << "Choose quiz type: (1) Running Count, (2) True Count: ";
        int choice;
        std::cin >> choice;
        
        if (choice == 1) {
            askRunningCountQuiz(playerName);
        } else if (choice == 2) {
            askTrueCountQuiz(playerName);
        } else {
            std::cout << "Invalid choice." << std::endl;
        }
    }
}

void Counting::displayCurrentCount() const {
    if (!countingEnabled) {
        std::cout << "Card counting is disabled." << std::endl;
        return;
    }
    
    std::cout << "\n=== CURRENT COUNT ===" << std::endl;
    std::cout << "System: " << getSystemName(currentSystem) << std::endl;
    std::cout << "Running Count: " << runningCount << std::endl;
    std::cout << "True Count: " << std::fixed << std::setprecision(2) << getTrueCount() << std::endl;
    std::cout << "Decks Remaining: " << getDecksRemaining() << std::endl;
    std::cout << "=====================" << std::endl;
}

void Counting::displayCountingAdvice() const {
    if (!countingEnabled) return;
    
    double trueCount = getTrueCount();
    std::cout << "\n--- COUNTING ADVICE ---" << std::endl;
    
    if (trueCount >= 2.0) {
        std::cout << "HIGH COUNT! Increase bets. Favor standing on close decisions." << std::endl;
    } else if (trueCount >= 1.0) {
        std::cout << "POSITIVE COUNT. Slight betting advantage." << std::endl;
    } else if (trueCount >= -1.0) {
        std::cout << "NEUTRAL COUNT. Play basic strategy." << std::endl;
    } else {
        std::cout << "NEGATIVE COUNT. Minimum bets recommended." << std::endl;
    }
    
    std::cout << "Betting Multiplier: " << getSuggestedBetMultiplier() << "x" << std::endl;
    std::cout << "----------------------" << std::endl;
}

void Counting::showCountingSystemInfo() const {
    std::cout << "\n======== COUNTING SYSTEM INFO ========" << std::endl;
    std::cout << "Current System: " << getSystemName(currentSystem) << std::endl;
    
    switch (currentSystem) {
        case CountingSystem::HI_LO:
            std::cout << "Hi-Lo System:\n";
            std::cout << "  +1: 2, 3, 4, 5, 6\n";
            std::cout << "   0: 7, 8, 9\n";
            std::cout << "  -1: 10, J, Q, K, A\n";
            std::cout << "Difficulty: Beginner\n";
            break;
        case CountingSystem::KO:
            std::cout << "Knock Out (KO) System:\n";
            std::cout << "  +1: 2, 3, 4, 5, 6, 7\n";
            std::cout << "   0: 8, 9\n";
            std::cout << "  -1: 10, J, Q, K, A\n";
            std::cout << "Difficulty: Beginner (Unbalanced)\n";
            break;
        case CountingSystem::HI_OPT_I:
            std::cout << "Hi-Opt I System:\n";
            std::cout << "  +1: 3, 4, 5, 6\n";
            std::cout << "   0: 2, 7, 8, 9, A\n";
            std::cout << "  -1: 10, J, Q, K\n";
            std::cout << "Difficulty: Intermediate\n";
            break;
        case CountingSystem::OMEGA_II:
            std::cout << "Omega II System:\n";
            std::cout << "  +2: 4, 5, 6\n";
            std::cout << "  +1: 2, 3, 7\n";
            std::cout << "   0: 8, A\n";
            std::cout << "  -1: 9\n";
            std::cout << "  -2: 10, J, Q, K\n";
            std::cout << "Difficulty: Advanced\n";
            break;
    }
    std::cout << "=======================================" << std::endl;
}

void Counting::displayPlayerCountingStats(const std::string& playerName) const {
    auto it = playerStats.find(playerName);
    if (it == playerStats.end()) {
        std::cout << "No counting statistics for " << playerName << std::endl;
        return;
    }
    
    const CountingStats& stats = it->second;
    
    std::cout << "\n=== COUNTING STATS: " << playerName << " ===" << std::endl;
    std::cout << "Total Quizzes: " << stats.totalQuizzes << std::endl;
    
    if (stats.totalRunningCountQuestions > 0) {
        double runningAccuracy = (static_cast<double>(stats.correctRunningCount) / 
                                 stats.totalRunningCountQuestions) * 100.0;
        std::cout << "Running Count Accuracy: " << std::fixed << std::setprecision(1) 
                  << runningAccuracy << "% (" << stats.correctRunningCount 
                  << "/" << stats.totalRunningCountQuestions << ")" << std::endl;
    }
    
    if (stats.totalTrueCountQuestions > 0) {
        double trueAccuracy = (static_cast<double>(stats.correctTrueCount) / 
                              stats.totalTrueCountQuestions) * 100.0;
        std::cout << "True Count Accuracy: " << std::fixed << std::setprecision(1) 
                  << trueAccuracy << "% (" << stats.correctTrueCount 
                  << "/" << stats.totalTrueCountQuestions << ")" << std::endl;
    }
    
    std::cout << "Average Response Time: " << std::fixed << std::setprecision(2) 
              << stats.averageResponseTime << " seconds" << std::endl;
    
    double overallAccuracy = getCountingAccuracy(playerName);
    std::cout << "Overall Counting Accuracy: " << std::fixed << std::setprecision(1) 
              << overallAccuracy << "%" << std::endl;
    
    if (overallAccuracy >= 90.0) {
        std::cout << "Rating: EXPERT! Excellent counting skills!" << std::endl;
    } else if (overallAccuracy >= 80.0) {
        std::cout << "Rating: ADVANCED. Very good counting ability." << std::endl;
    } else if (overallAccuracy >= 70.0) {
        std::cout << "Rating: INTERMEDIATE. Good progress, keep practicing!" << std::endl;
    } else if (overallAccuracy >= 60.0) {
        std::cout << "Rating: BEGINNER. Focus on accuracy over speed." << std::endl;
    } else {
        std::cout << "Rating: NEEDS PRACTICE. Consider starting with easier system." << std::endl;
    }
    
    std::cout << "========================================" << std::endl;
}

void Counting::displayAllCountingStats() const {
    std::cout << "\n======== ALL COUNTING STATISTICS ========" << std::endl;
    
    if (playerStats.empty()) {
        std::cout << "No counting statistics recorded yet." << std::endl;
        std::cout << "==========================================" << std::endl;
        return;
    }
    
    for (const auto& pair : playerStats) {
        displayPlayerCountingStats(pair.first);
    }
}

void Counting::resetCountingStats() {
    playerStats.clear();
    std::cout << "All counting statistics have been reset!" << std::endl;
}

void Counting::resetCountingStats(const std::string& playerName) {
    playerStats.erase(playerName);
    std::cout << "Counting statistics for " << playerName << " have been reset!" << std::endl;
}

std::string Counting::getBettingAdvice() const {
    if (!countingEnabled) return "Counting disabled - use flat betting";
    
    double trueCount = getTrueCount();
    
    if (trueCount >= 3.0) {
        return "VERY HIGH COUNT - Maximum bets recommended";
    } else if (trueCount >= 2.0) {
        return "HIGH COUNT - Increase bets significantly";
    } else if (trueCount >= 1.0) {
        return "POSITIVE COUNT - Moderate bet increase";
    } else if (trueCount >= -1.0) {
        return "NEUTRAL COUNT - Minimum bets";
    } else {
        return "NEGATIVE COUNT - Minimum bets, consider leaving table";
    }
}

int Counting::getSuggestedBetMultiplier() const {
    if (!countingEnabled) return 1;
    
    double trueCount = getTrueCount();
    
    if (trueCount >= 3.0) return 5;      // 5x minimum bet
    else if (trueCount >= 2.0) return 4; // 4x minimum bet
    else if (trueCount >= 1.5) return 3; // 3x minimum bet
    else if (trueCount >= 1.0) return 2; // 2x minimum bet
    else return 1;                       // 1x minimum bet
}

void Counting::enterTrainingMode() {
    std::cout << "\n========== COUNTING TRAINING MODE ==========" << std::endl;
    std::cout << "Practice your counting skills!" << std::endl;
    std::cout << "Current system: " << getSystemName(currentSystem) << std::endl;
    
    bool training = true;
    while (training) {
        std::cout << "\nTraining Options:" << std::endl;
        std::cout << "1. Practice drill (20 cards)" << std::endl;
        std::cout << "2. Custom drill" << std::endl;
        std::cout << "3. System information" << std::endl;
        std::cout << "4. Change counting system" << std::endl;
        std::cout << "5. Exit training" << std::endl;
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                practiceCountingDrill(20);
                break;
            case 2: {
                std::cout << "How many cards? ";
                int cards;
                std::cin >> cards;
                practiceCountingDrill(cards);
                break;
            }
            case 3:
                showCountingSystemInfo();
                break;
            case 4: {
                std::cout << "Choose system: (1) Hi-Lo (2) KO (3) Hi-Opt I (4) Omega II: ";
                int sys;
                std::cin >> sys;
                if (sys >= 1 && sys <= 4) {
                    setCountingSystem(static_cast<CountingSystem>(sys - 1));
                }
                break;
            }
            case 5:
                training = false;
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
        }
    }
    
    std::cout << "Exiting training mode." << std::endl;
}

void Counting::practiceCountingDrill(int numberOfCards) {
    std::cout << "\n--- COUNTING DRILL: " << numberOfCards << " cards ---" << std::endl;
    std::cout << "Keep track of the count as cards are shown." << std::endl;
    std::cout << "Press Enter to see each card..." << std::endl;
    
    std::cin.ignore(); // Clear input buffer
    
    int practiceCount = 0;
    std::vector<Card> drillCards;
    
    // Generate random cards for the drill
    std::uniform_int_distribution<int> suitDist(0, 3);
    std::uniform_int_distribution<int> valueDist(1, 13);
    
    for (int i = 0; i < numberOfCards; ++i) {
        Card card(valueDist(rng), static_cast<Suit>(suitDist(rng)));
        drillCards.push_back(card);
        
        std::cout << "Card " << (i + 1) << ": " << card.toString();
        int cardValue = getCardCountValue(card, currentSystem);
        practiceCount += cardValue;
        
        std::cout << " (Value: " << (cardValue > 0 ? "+" : "") << cardValue << ")";
        std::cout << std::endl;
        
        std::cin.get(); // Wait for Enter
    }
    
    std::cout << "\nDrill complete! What is your final count? ";
    int playerCount;
    std::cin >> playerCount;
    
    if (playerCount == practiceCount) {
        std::cout << "CORRECT! The count is " << practiceCount << std::endl;
    } else {
        std::cout << "Incorrect. The correct count is " << practiceCount 
                  << " (you said " << playerCount << ")" << std::endl;
    }
    
    double accuracy = (playerCount == practiceCount) ? 100.0 : 0.0;
    std::cout << "Drill accuracy: " << accuracy << "%" << std::endl;
}

double Counting::getCountingAccuracy(const std::string& playerName) const {
    auto it = playerStats.find(playerName);
    if (it == playerStats.end()) return 0.0;
    
    const CountingStats& stats = it->second;
    int totalCorrect = stats.correctRunningCount + stats.correctTrueCount;
    int totalQuestions = stats.totalRunningCountQuestions + stats.totalTrueCountQuestions;
    
    if (totalQuestions == 0) return 0.0;
    
    return (static_cast<double>(totalCorrect) / totalQuestions) * 100.0;
}

bool Counting::isPlayerCountingAccurately(const std::string& playerName, double threshold) const {
    return getCountingAccuracy(playerName) >= threshold;
}

void Counting::processDealtCards(const std::vector<Card>& cards) {
    for (const Card& card : cards) {
        updateCount(card);
    }
}

bool Counting::checkForQuizOpportunity(const std::string& currentPlayer) {
    if (shouldTriggerQuiz()) {
        triggerRandomQuiz(currentPlayer);
        return true;
    }
    return false;
}