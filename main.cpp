#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <vector>
#include "GameEngine.h"
#include "basicStrag.h"
#include "counting.h"

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void displayWelcome() {
    std::cout << "======================================" << std::endl;
    std::cout << "       WELCOME TO BLACKJACK!         " << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "Rules:" << std::endl;
    std::cout << "- Get as close to 21 as possible without going over" << std::endl;
    std::cout << "- Aces count as 1 or 11, face cards count as 10" << std::endl;
    std::cout << "- Dealer hits on 16 and stands on 17" << std::endl;
    std::cout << "- Blackjack = 21 with 2 cards" << std::endl;
    std::cout << "- Using 6-deck shoe (312 cards total)" << std::endl;
    std::cout << "======================================" << std::endl;
}

std::string getPlayerName() {
    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);
    
    // Default name if empty
    if (name.empty()) {
        name = "Player";
    }
    
    return name;
}

void displayCurrentPlayers(const std::vector<std::string>& playerNames) {
    if (playerNames.empty()) {
        std::cout << "No players registered yet." << std::endl;
        return;
    }
    
    std::cout << "\nCurrent players:" << std::endl;
    for (size_t i = 0; i < playerNames.size(); ++i) {
        std::cout << (i + 1) << ". " << playerNames[i] << std::endl;
    }
}

int getPlayerManagementChoice(bool hasPlayers) {
    std::cout << "\n======================================" << std::endl;
    std::cout << "Player Management:" << std::endl;
    
    if (hasPlayers) {
        std::cout << "1. Play with current players" << std::endl;
        std::cout << "2. Add new player" << std::endl;
        std::cout << "3. Remove a player" << std::endl;
        std::cout << "4. Clear all players and start fresh" << std::endl;
        std::cout << "5. View current players" << std::endl;
        std::cout << "6. Back to main menu" << std::endl;
    } else {
        std::cout << "1. Add new player" << std::endl;
        std::cout << "2. Back to main menu" << std::endl;
    }
    std::cout << "======================================" << std::endl;
    
    int choice;
    std::cout << "Enter your choice: ";
    std::cin >> choice;
    
    if (std::cin.fail()) {
        clearInput();
        return -1;
    }
    
    clearInput(); // Clear the newline after the number
    return choice;
}

bool removePlayer(std::vector<std::string>& playerNames) {
    if (playerNames.empty()) {
        std::cout << "No players to remove!" << std::endl;
        return false;
    }
    
    displayCurrentPlayers(playerNames);
    
    int choice;
    std::cout << "\nEnter the number of the player to remove (0 to cancel): ";
    std::cin >> choice;
    
    if (std::cin.fail()) {
        clearInput();
        std::cout << "Invalid input." << std::endl;
        return false;
    }
    
    clearInput();
    
    if (choice == 0) {
        return false;
    }
    
    if (choice < 1 || choice > static_cast<int>(playerNames.size())) {
        std::cout << "Invalid player number." << std::endl;
        return false;
    }
    
    std::string removedPlayer = playerNames[choice - 1];
    playerNames.erase(playerNames.begin() + choice - 1);
    std::cout << "Removed player: " << removedPlayer << std::endl;
    return true;
}

void displayGameMenu() {
    std::cout << "\n======================================" << std::endl;
    std::cout << "Game Options:" << std::endl;
    std::cout << "1. Manage Players / Start Game" << std::endl;
    std::cout << "2. Configure Game Features (Counting/Strategy)" << std::endl;
    std::cout << "3. Basic Strategy Trainer" << std::endl;
    std::cout << "4. Card Counting Trainer" << std::endl;
    std::cout << "5. View Game Statistics" << std::endl;
    std::cout << "6. Reset Statistics" << std::endl;
    std::cout << "7. View Rules" << std::endl;
    std::cout << "8. Exit" << std::endl;
    std::cout << "======================================" << std::endl;
}

void displayRules() {
    std::cout << "\n======================================" << std::endl;
    std::cout << "           BLACKJACK RULES            " << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "OBJECTIVE:" << std::endl;
    std::cout << "- Beat the dealer by getting closer to 21 without going over" << std::endl;
    std::cout << std::endl;
    std::cout << "CARD VALUES:" << std::endl;
    std::cout << "- Number cards (2-10): Face value" << std::endl;
    std::cout << "- Face cards (J, Q, K): 10 points" << std::endl;
    std::cout << "- Aces: 1 or 11 points (whichever is better)" << std::endl;
    std::cout << std::endl;
    std::cout << "GAMEPLAY:" << std::endl;
    std::cout << "- Each player and dealer get 2 cards initially" << std::endl;
    std::cout << "- Player cards are face-up, dealer has one face-down (hole card)" << std::endl;
    std::cout << "- Players choose to 'Hit' (take another card) or 'Stand' (keep current total)" << std::endl;
    std::cout << "- Additional options: Double Down, Split pairs, Surrender" << std::endl;
    std::cout << "- If you go over 21, you 'bust' and lose immediately" << std::endl;
    std::cout << "- Dealer reveals hole card and hits until reaching 17 or higher" << std::endl;
    std::cout << std::endl;
    std::cout << "WINNING:" << std::endl;
    std::cout << "- Blackjack (21 with 2 cards) beats regular 21" << std::endl;
    std::cout << "- Higher total wins (without busting)" << std::endl;
    std::cout << "- Same total = Push (tie)" << std::endl;
    std::cout << std::endl;
    std::cout << "DECK INFO:" << std::endl;
    std::cout << "- Uses 6-deck shoe (312 cards total)" << std::endl;
    std::cout << "- Same shoe continues across multiple games" << std::endl;
    std::cout << "- Reshuffles when cut card is reached (60-80 cards remaining)" << std::endl;
    std::cout << "======================================" << std::endl;
}

bool playAnotherGame() {
    char choice;
    do {
        std::cout << "\nWould you like to play another game with the same players? (y/n): ";
        std::cin >> choice;
        
        if (std::cin.fail()) {
            clearInput();
            continue;
        }
        
        choice = std::tolower(choice);
        
        if (choice != 'y' && choice != 'n') {
            std::cout << "Please enter 'y' for yes or 'n' for no." << std::endl;
        }
    } while (choice != 'y' && choice != 'n');
    
    return choice == 'y';
}

void startGameWithPlayers(const std::vector<std::string>& playerNames, Deck& gameDeck, GameEngine& gameEngine){
    if (playerNames.empty()) {
        std::cout << "No players to start the game with!" << std::endl;
        return;
    }
    
    bool continuePlaying = true;
    
    while (continuePlaying) { 
        gameEngine.clearPlayers();
        // Add all registered players
        for (const auto& name : playerNames) {
            gameEngine.addPlayer(name);
        }
        
        // Play the game
        std::cout << "\nStarting game with registered players..." << std::endl;
        gameEngine.playGame();
        
        // Check if deck needs reshuffling after the game
        if (gameEngine.isDeckReshuffleNeeded()) {
            std::cout << "\n*** Cut card reached! ***" << std::endl;
            gameDeck.cutDeck();  // Reshuffle the deck
        }
        
        // Ask if they want to play again with same players
        if (!playAnotherGame()) {
            continuePlaying = false;
        }
    }
}

void basicStrategyTrainer(Deck& /* deck */) {
    BasicStrat strategy;
    std::cout << "\n=== BASIC STRATEGY TRAINER ===" << std::endl;
    std::cout << "Practice your basic strategy decisions!" << std::endl;
    
    bool training = true;
    while (training) {
        std::cout << "\nStrategy Trainer Options:" << std::endl;
        std::cout << "1. View strategy recommendations" << std::endl;
        std::cout << "2. View your strategy statistics" << std::endl;
        std::cout << "3. Reset strategy statistics" << std::endl;
        std::cout << "4. Return to main menu" << std::endl;
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        
        if (std::cin.fail()) {
            clearInput();
            continue;
        }
        
        switch (choice) {
            case 1:
                std::cout << "Strategy recommendations are provided during regular gameplay." << std::endl;
                break;
            case 2:
                strategy.displayAllStats();
                break;
            case 3:
                strategy.resetStats();
                break;
            case 4:
                training = false;
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
                break;
        }
    }
}

void cardCountingTrainer(Deck& deck) {
    Counting counting(&deck);
    std::cout << "\n=== CARD COUNTING TRAINER ===" << std::endl;
    
    bool training = true;
    while (training) {
        std::cout << "\nCounting Trainer Options:" << std::endl;
        std::cout << "1. Enter training mode" << std::endl;
        std::cout << "2. Enable counting for games" << std::endl;
        std::cout << "3. Disable counting" << std::endl;
        std::cout << "4. View counting statistics" << std::endl;
        std::cout << "5. Reset counting statistics" << std::endl;
        std::cout << "6. Return to main menu" << std::endl;
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        
        if (std::cin.fail()) {
            clearInput();
            continue;
        }
        
        switch (choice) {
            case 1:
                counting.enterTrainingMode();
                break;
            case 2:
                counting.enableCounting(true);
                break;
            case 3:
                counting.enableCounting(false);
                break;
            case 4:
                counting.displayAllCountingStats();
                break;
            case 5:
                counting.resetCountingStats();
                break;
            case 6:
                training = false;
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
                break;
        }
    }
}

void configureGameFeatures(GameEngine& gameEngine) {
    bool configuring = true;
    
    while (configuring) {
        std::cout << "\n======== GAME FEATURES CONFIGURATION ========" << std::endl;
        std::cout << "1. Enable/Disable Card Counting" << std::endl;
        std::cout << "2. Change Counting System" << std::endl;
        std::cout << "3. Enable/Disable Basic Strategy Tracking" << std::endl;
        std::cout << "4. View Current Count" << std::endl;
        std::cout << "5. View Strategy Statistics" << std::endl;
        std::cout << "6. Back to Main Menu" << std::endl;
        std::cout << "=============================================" << std::endl;
        
        int choice;
        std::cout << "Enter your choice (1-6): ";
        std::cin >> choice;
        
        if (std::cin.fail()) {
            clearInput();
            continue;
        }
        
        clearInput();
        
        switch (choice) {
            case 1: {
                std::cout << "Enable card counting? (y/n): ";
                char enable;
                std::cin >> enable;
                gameEngine.enableCounting(std::tolower(enable) == 'y');
                break;
            }
            case 2: {
                std::cout << "Choose counting system:" << std::endl;
                std::cout << "1. Hi-Lo (Beginner)" << std::endl;
                std::cout << "2. KO (Knock Out)" << std::endl;
                std::cout << "3. Hi-Opt I (Intermediate)" << std::endl;
                std::cout << "4. Omega II (Advanced)" << std::endl;
                std::cout << "Choice: ";
                int system;
                std::cin >> system;
                if (system >= 1 && system <= 4) {
                    gameEngine.setCountingSystem(static_cast<CountingSystem>(system - 1));
                }
                break;
            }
            case 3: {
                std::cout << "Basic strategy tracking is always enabled during games." << std::endl;
                std::cout << "It automatically tracks your decisions vs optimal play." << std::endl;
                break;
            }
            case 4: {
                if (gameEngine.getCountingSystem()) {
                    gameEngine.getCountingSystem()->displayCurrentCount();
                }
                break;
            }
            case 5: {
                if (gameEngine.getBasicStrategy()) {
                    gameEngine.getBasicStrategy()->displayAllStats();
                }
                break;
            }
            case 6:
                configuring = false;
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
                break;
        }
    }
}

int main() {
    displayWelcome();
    
    // Create the 6-deck shoe once - this persists throughout the session
    Deck gameDeck;
    std::cout << "\n6-deck shoe initialized and shuffled!" << std::endl;
    
    // Persistent player list
    std::vector<std::string> playerNames;
    GameEngine gameSession(gameDeck);
    
    bool keepPlaying = true;
    
    while (keepPlaying) {
        displayGameMenu();
        
        int choice;
        std::cout << "Enter your choice (1-7): ";
        std::cin >> choice;
        
        if (std::cin.fail()) {
            clearInput();
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }
        
        clearInput();
        
        switch (choice) {
            case 1: {
                // Player Management / Start Game
                bool inPlayerMenu = true;
                
                while (inPlayerMenu) {
                    displayCurrentPlayers(playerNames);
                    
                    int playerChoice = getPlayerManagementChoice(!playerNames.empty());
                    
                    if (playerNames.empty()) {
                        switch (playerChoice) {
                            case 1: {
                                std::string name = getPlayerName();
                                playerNames.push_back(name);
                                std::cout << "Added player: " << name << std::endl;
                                break;
                            }
                            case 2:
                                inPlayerMenu = false;
                                break;
                            default:
                                std::cout << "Invalid choice." << std::endl;
                                break;
                        }
                    } else {
                        switch (playerChoice) {
                            case 1:
                                startGameWithPlayers(playerNames, gameDeck, gameSession);
                                break;
                            case 2: {
                                if (playerNames.size() >= 4) {
                                    std::cout << "Maximum of 4 players allowed!" << std::endl;
                                } else {
                                    std::string name = getPlayerName();
                                    playerNames.push_back(name);
                                    std::cout << "Added player: " << name << std::endl;
                                }
                                break;
                            }
                            case 3:
                                removePlayer(playerNames);
                                break;
                            case 4:
                                playerNames.clear();
                                std::cout << "All players removed." << std::endl;
                                break;
                            case 5:
                                break;
                            case 6:
                                inPlayerMenu = false;
                                break;
                            default:
                                std::cout << "Invalid choice." << std::endl;
                                break;
                        }
                    }
                }
                break;
            }
            case 2:
                configureGameFeatures(gameSession);
                break;
            case 3:
                basicStrategyTrainer(gameDeck);
                break;
            case 4:
                cardCountingTrainer(gameDeck);
                break;
            case 5:
                gameSession.displayGameStats();
                break;
            case 6:
                gameSession.resetGameStats();
                break;
            case 7:
                displayRules();
                break;
            case 8:
                std::cout << "\n=== ENDING BLACKJACK SESSION ===" << std::endl;
                gameSession.displayGameStats();
                std::cout << "\nThanks for playing Blackjack! Goodbye!" << std::endl;
                keepPlaying = false;
                break;
            default:
                std::cout << "Invalid choice. Please enter 1-8." << std::endl;
                break;
        }
    }
    

    
    
    return 0;
}