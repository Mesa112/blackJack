#include "GameEngine.h"
#include <iostream>
#include <iomanip>
#include <cctype>

GameEngine::GameEngine(Deck& gameDeck) 
    : deck(gameDeck), dealer(), currentState(GameState::SETUP), 
      currentPlayerIndex(0), actionHandler(nullptr), display(nullptr) {
    
    // Initialize split manager with smart pointer
    splitManager = std::make_unique<SplitHand>(players, deck, 4);

    countingSystem = std::make_unique<Counting>(&deck);
    basicStrategy = std::make_unique<BasicStrat>();
}

void GameEngine::addPlayer(const std::string& playerName) {
    players.emplace_back(playerName);
    std::cout << "Added player: " << playerName << std::endl;
}

void GameEngine::startNewGame() {
    // Clear previous game state
    playerHasSurrendered.clear();
    currentState = GameState::SETUP;
    currentPlayerIndex = 0;
    
    // Clear all hands from previous game
    for (auto& player : players) {
        player.clearHand();
    }
    dealer.clearHand();
    dealer.resetForNewGame();
    
    if (deck.isThresholdReached()) {
        std::cout << "Note: Cut card has been reached. This will be the last hand before reshuffle." << std::endl;
    }
    
    // Increment games played counter
    gameStats.incrementGamesPlayed();
}

void GameEngine::dealInitialCards() {
    currentState = GameState::DEALING;
    
    // Deal two cards to each player and update count
    for (auto& player : players) {
        Card card1 = deck.dealCard();
        Card card2 = deck.dealCard();
        player.addCard(card1);
        player.addCard(card2);
        
        // Update counting system
        if (countingSystem && countingSystem->isCountingEnabled()) {
            countingSystem->updateCount(card1);
            countingSystem->updateCount(card2);
        }
    }
    
    // Deal two cards to dealer and update count
    Card dealerCard1 = deck.dealCard();
    Card dealerCard2 = deck.dealCard();
    dealer.addCard(dealerCard1);
    dealer.addCard(dealerCard2);
    
    // Update counting system (only count visible dealer card)
    if (countingSystem && countingSystem->isCountingEnabled()) {
        countingSystem->updateCount(dealerCard1);
    }
}

void GameEngine::playPlayerTurn(int playerIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
        return;
    }
    
    Player& player = players[playerIndex];
    
    std::cout << "\n=== " << player.getName() << "'s Turn ===" << std::endl;
    displayPlayerHand(player);

    if (player.isBlackjack()) {
        return;
    }

    bool playerTurnActive = true;
    bool firstTurn = true;
    
    while (!player.isBusted() && playerTurnActive) {
        bool canDouble = (firstTurn && player.getCardCount() == 2);
        bool canSurrender = (firstTurn && player.getCardCount() == 2);
        bool canSplit = (firstTurn && canPlayerSplit(playerIndex));

        std::cout << "\nChoose action: (h)it, (s)tand";
        if(canDouble) std::cout << ", (d)ouble down";
        if(canSurrender) std::cout << ", s(u)rrender";
        if(canSplit) std::cout << ", s(p)lit";
        std::cout << ": ";

        char choice;
        std::cin >> choice;
        choice = std::tolower(choice);

        if (choice == 'h') {
            if (!playerHit(playerIndex)) {
                displayPlayerHand(player);
                playerTurnActive = false;
            }else{
                displayPlayerHand(player);
            }
            firstTurn = false;
            
        } else if (choice == 's') {
            playerStand(playerIndex);
            playerTurnActive = false;
            
        } else if (choice == 'd' && canDouble) {
            if (!playerDoublesDown(playerIndex)) {
                // Player busted after doubling
            }
            displayPlayerHand(player);
            playerTurnActive = false;
            
        } else if (choice == 'u' && canSurrender) {
            if (playerSurrenders(playerIndex)) {
                playerTurnActive = false;
            }
            
        } else if (choice == 'p' && canSplit) {
            if (playerSplits(playerIndex)) {
                playerTurnActive = false;  // Split hands are handled in playerSplits()
            }
            
        } else {
            std::cout << "Invalid choice. Available options: h, s";
            if (canDouble) std::cout << ", d";
            if (canSurrender) std::cout << ", u";
            if (canSplit) std::cout << ", p";
            std::cout << std::endl;
        }
    }
}

void GameEngine::playDealerTurn() {
    currentState = GameState::DEALER_TURN;

    int playersStillOn = 0;
    for(size_t i = 0; i < players.size(); i++){
        const Player& player = players[i];
        if(playerHasSurrendered.find(i) != playerHasSurrendered.end() && 
           playerHasSurrendered.at(i)) continue;
        if(!player.isBusted()) playersStillOn++;
    }

    if(!playersStillOn) return;
    
    dealer.revealHoleCard();
    
    // Now count the hole card since it's revealed
    if (countingSystem && countingSystem->isCountingEnabled() && dealer.getCardCount() > 1) {
        countingSystem->updateCount(dealer.getCard(1)); // Count the hole card
    }
    
    std::cout << "\nDealer reveals hole card:" << std::endl;
    displayDealerHand(true);
    
    // Dealer hits until 17 or higher
    while (dealer.shouldHit()) {
        std::cout << "\nDealer hits..." << std::endl;
        Card newCard = deck.dealCard();
        dealer.addCard(newCard);
        
        // Update counting system
        if (countingSystem && countingSystem->isCountingEnabled()) {
            countingSystem->updateCount(newCard);
        }
        
        displayDealerHand(true);
        
        if (dealer.getTotalValue() > 21) {
            std::cout << "Dealer busts!" << std::endl;
            break;
        }
    }
    
    if (dealer.getTotalValue() <= 21) {
        std::cout << "Dealer stands on " << dealer.getTotalValue() << std::endl;
    }
}

void GameEngine::endGame() {
    currentState = GameState::GAME_OVER;
    
    // Update game statistics
    updateGameStats();
    
    // Check if reshuffle is needed
    if (isDeckReshuffleNeeded()) {
        std::cout << "\n*** CUT CARD REACHED ***" << std::endl;
        std::cout << "The shoe will be reshuffled before the next game." << std::endl;
    }
}

bool GameEngine::canPlayerSplit(int playerIndex) {
    return splitManager->canPlayerSplit(playerIndex);
}

bool GameEngine::playerSplits(int playerIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
        return false;
    }
    
    Player& player = players[playerIndex];
    
    // Record the strategy decision
    if (basicStrategy) {
        Action takenAction = Action::SPLIT;
        basicStrategy->recordPlayerAction(player.getName(), player, dealer, 
                                        takenAction, true, true);
    }
    
    return splitManager->playerSplits(playerIndex);
}

bool GameEngine::playerHit(int playerIndex) {
    if (playerIndex >= 0 && playerIndex < static_cast<int>(players.size())) {
        Player& player = players[playerIndex];
        
        // Record the strategy decision if basic strategy is enabled
        if (basicStrategy) {
            Action takenAction = Action::HIT;
            basicStrategy->recordPlayerAction(player.getName(), player, dealer, 
                                            takenAction, true, true);
        }
        
        Card newCard = deck.dealCard();
        player.addCard(newCard);
        
        // Update counting system
        if (countingSystem && countingSystem->isCountingEnabled()) {
            countingSystem->updateCount(newCard);
        }
        
        return player.getTotalValue() <= 21;
    }
    return false;
}


bool GameEngine::playerStand(int playerIndex) {
    if (playerIndex >= 0 && playerIndex < static_cast<int>(players.size())) {
        Player& player = players[playerIndex];
        
        // Record the strategy decision
        if (basicStrategy) {
            Action takenAction = Action::STAND;
            basicStrategy->recordPlayerAction(player.getName(), player, dealer, 
                                            takenAction, true, true);
        }
        return true;
    }
    return false;
}

bool GameEngine::playerDoublesDown(int playerIndex) {
    if(playerIndex >= 0 && playerIndex < static_cast<int>(players.size())) {
        Player& player = players[playerIndex];
        
        // Record the strategy decision
        if (basicStrategy) {
            Action takenAction = Action::DOUBLE;
            basicStrategy->recordPlayerAction(player.getName(), player, dealer, 
                                            takenAction, true, true);
        }
        
        Card newCard = deck.dealCard();
        player.addCard(newCard);
        
        // Update counting system
        if (countingSystem && countingSystem->isCountingEnabled()) {
            countingSystem->updateCount(newCard);
        }
        
        bool didNotBust = player.getTotalValue() <= 21;
        
        if (didNotBust) {
            std::cout << player.getName() << " doubles down and stands on " 
                      << player.getTotalValue() << std::endl;
        } else {
            std::cout << player.getName() << " doubles down and busts with " 
                      << player.getTotalValue() << std::endl;
        }
        return didNotBust;
    }
    return false;
}

bool GameEngine::playerSurrenders(int playerIndex) {
    if (playerIndex < 0 || playerIndex >= static_cast<int>(players.size())) {
        return false;
    }
    
    Player& player = players[playerIndex];
    
    // Record the strategy decision
    if (basicStrategy) {
        Action takenAction = Action::SURRENDER;
        basicStrategy->recordPlayerAction(player.getName(), player, dealer, 
                                        takenAction, true, true);
    }
    
    std::cout << player.getName() << " surrenders and loses half their bet." << std::endl;
    playerHasSurrendered[playerIndex] = true;
    return true;
}

void GameEngine::displayPlayerHand(const Player& player) const {
    std::cout << player.getName() << "'s hand: ";
    
    for (size_t i = 0; i < player.getCardCount(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << player.getCard(i).toString();
    }
    
    std::cout << " (Total: " << player.getTotalValue() << ")";
    
    if (player.isBlackjack()) {
        std::cout << " - BLACKJACK!";
    } else if (player.isBusted()) {
        std::cout << " - BUST!";
    }
    
    std::cout << std::endl;
}

void GameEngine::displayDealerHand(bool showHoleCard) const {
    std::cout << "Dealer's hand: ";
    
    if (dealer.getCardCount() == 0) {
        std::cout << "No cards" << std::endl;
        return;
    }
    
    // Always show the first card (up card)
    std::cout << dealer.getCard(0).toString();
    
    if (dealer.getCardCount() > 1) {
        if (showHoleCard || dealer.isHoleCardRevealed()) {
            std::cout << ", " << dealer.getCard(1).toString();
            // Show any additional cards
            for (size_t i = 2; i < dealer.getCardCount(); ++i) {
                std::cout << ", " << dealer.getCard(i).toString();
            }
        } else {
            std::cout << ", [Hidden Card]";
        }
    }
    
    if (showHoleCard || dealer.isHoleCardRevealed()) {
        std::cout << " (Total: " << dealer.getTotalValue() << ")";
    } else {
        // Only show the value of the up card when hole card is hidden
        int upCardValue = dealer.getCard(0).getValue();
        if (upCardValue == 1) upCardValue = 11; // Show Ace as 11 initially
        else if (upCardValue > 10) upCardValue = 10; // Face cards
        std::cout << " (Showing: " << upCardValue << ")";
    }
    
    std::cout << std::endl;
}

void GameEngine::displayResults() const {
    std::cout << "\n=== FINAL RESULTS ===" << std::endl;
    bool dealerPlayed = (currentState == GameState::GAME_OVER && 
        dealer.isHoleCardRevealed());

    displayDealerHand(dealerPlayed);
    
    for (size_t i = 0; i < players.size(); ++i) {
        const Player& player = players[i];
        
        // Check if this specific player surrendered
        if (playerHasSurrendered.find(i) != playerHasSurrendered.end() && 
            playerHasSurrendered.at(i)) {
            std::cout << "Result: " << player.getName() 
                      << " surrendered and lost half their bet." << std::endl;
            continue;
        }
        
        displayPlayerHand(player);
        
        // Determine result for non-surrendered players
        if (player.isBusted()) {
            std::cout << "Result: " << player.getName() << " loses (bust)" << std::endl;
        } else if (dealer.isBusted()) {
            std::cout << "Result: " << player.getName() << " wins (dealer bust)" << std::endl;
        } else if (player.isBlackjack() && !dealer.isBlackjack()) {
            std::cout << "Result: " << player.getName() << " wins with blackjack!" << std::endl;
        } else if (dealer.isBlackjack() && !player.isBlackjack()) {
            std::cout << "Result: " << player.getName() << " loses (dealer blackjack)" << std::endl;
        } else if (player.getTotalValue() > dealer.getTotalValue()) {
            std::cout << "Result: " << player.getName() << " wins!" << std::endl;
        } else if (player.getTotalValue() < dealer.getTotalValue()) {
            std::cout << "Result: " << player.getName() << " loses" << std::endl;
        } else {
            std::cout << "Result: " << player.getName() << " pushes (tie)" << std::endl;
        }
        std::cout << std::endl;
    }
}

void GameEngine::updateGameStats() {
    for (size_t i = 0; i < players.size(); ++i) {
        const Player& player = players[i];
        std::string playerName = player.getName();
        
        // Check if player surrendered first
        if (playerHasSurrendered.find(i) != playerHasSurrendered.end() && 
            playerHasSurrendered.at(i)) {
            gameStats.updatePlayerStats(playerName, GameResult::LOSS);
            continue;
        }
        
        // Check if this player has split hands
        if (splitManager->getPlayerSplitHands().find(i) != splitManager->getPlayerSplitHands().end()) {
            // Handle split hands
            auto& splitHands = splitManager->getPlayerSplitHands().at(i);
            
            for (size_t handIndex = 0; handIndex < splitHands.size(); ++handIndex) {
                const auto& hand = splitHands[handIndex];
                
                if (hand.isBusted()) {
                    gameStats.updatePlayerStats(playerName, GameResult::LOSS);
                } else if (hand.is21() && !dealer.isBlackjack()) {
                    gameStats.updatePlayerStats(playerName, GameResult::WIN);
                } else if (dealer.isBusted()) {
                    gameStats.updatePlayerStats(playerName, GameResult::WIN);
                } else if (hand.is21() && dealer.isBlackjack()) {
                    gameStats.updatePlayerStats(playerName, GameResult::PUSH);
                } else if (hand.getTotalValue() > dealer.getTotalValue()) {
                    gameStats.updatePlayerStats(playerName, GameResult::WIN);
                } else if (hand.getTotalValue() < dealer.getTotalValue()) {
                    gameStats.updatePlayerStats(playerName, GameResult::LOSS);
                } else {
                    gameStats.updatePlayerStats(playerName, GameResult::PUSH);
                }
            }
        } else {
            // Handle regular (non-split) hands
            if (player.isBusted()) {
                gameStats.updatePlayerStats(playerName, GameResult::LOSS);
            } else if (player.isBlackjack() && !dealer.isBlackjack()) {
                gameStats.updatePlayerStats(playerName, GameResult::BLACKJACK);
            } else if (dealer.isBusted()) {
                gameStats.updatePlayerStats(playerName, GameResult::WIN);
            } else if (player.isBlackjack() && dealer.isBlackjack()) {
                gameStats.updatePlayerStats(playerName, GameResult::PUSH);
            } else if (player.getTotalValue() > dealer.getTotalValue()) {
                gameStats.updatePlayerStats(playerName, GameResult::WIN);
            } else if (player.getTotalValue() < dealer.getTotalValue()) {
                gameStats.updatePlayerStats(playerName, GameResult::LOSS);
            } else {
                gameStats.updatePlayerStats(playerName, GameResult::PUSH);
            }
        }
    }
}

void GameEngine::playGame() {
    if (players.empty()) {
        std::cout << "No players added to the game!" << std::endl;
        return;
    }
    
    // Start new game
    startNewGame();
    
    // Deal initial cards
    dealInitialCards();
    
    // Display initial state
    std::cout << "\n=== INITIAL CARDS ===" << std::endl;
    displayDealerHand(false);
    
    for (size_t i = 0; i < players.size(); ++i) {
        displayPlayerHand(players[i]);
        if (players[i].isBlackjack()) {
            std::cout << "*** BLACKJACK! ***" << std::endl;
        }
    }
    
    // Check for dealer blackjack
    bool dealerHasBlackjack = dealer.checkForBlackjack();
    if (dealerHasBlackjack) {
        std::cout << "\nDealer has blackjack!" << std::endl;
        dealer.revealHoleCard();
        displayDealerHand(true);
        displayResults();
        endGame();
        return;
    }
    
    // Player turns
    currentState = GameState::PLAYER_TURNS;
    for (size_t i = 0; i < players.size(); ++i) {
        currentPlayerIndex = static_cast<int>(i);
        
        if (players[i].isBlackjack()) {
            std::cout << "\n" << players[i].getName() << " has blackjack and stands." << std::endl;
            continue;
        }
        
        playPlayerTurn(static_cast<int>(i));
    }
    
    // Dealer turn
    playDealerTurn();
    
    // Show results
    displayResults();
    
    // End game
    endGame();
}

// Getter methods
GameState GameEngine::getCurrentState() const {
    return currentState;
}

bool GameEngine::isGameOver() const {
    return currentState == GameState::GAME_OVER;
}

int GameEngine::getCurrentPlayerIndex() const {
    return currentPlayerIndex;
}

bool GameEngine::isDeckReshuffleNeeded() const {
    return deck.isThresholdReached();
}

// Statistics methods
void GameEngine::displayGameStats() const {
    gameStats.displayFinalResults(countingSystem.get(), basicStrategy.get());
}

void GameEngine::resetGameStats() {
    gameStats.resetStats();
}

void GameEngine::clearPlayers() {
    players.clear();
}

void GameEngine::enableCounting(bool enabled) {
    if (countingSystem) {
        countingSystem->enableCounting(enabled);
    }
}

void GameEngine::enableBasicStrategy(bool enabled) {
    (void)enabled;
}

void GameEngine::setCountingSystem(CountingSystem system) {
    if (countingSystem) {
        countingSystem->setCountingSystem(system);
    }
}