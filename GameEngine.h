#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include "deck.h"
#include "player.h"
#include "Dealer.h"
#include "Stats.h"
#include "SplitHand.h"
#include "counting.h"
#include "basicStrag.h"

enum class GameState {
    SETUP,
    DEALING,
    PLAYER_TURNS,
    DEALER_TURN,
    GAME_OVER
};

// Forward declarations for classes that are only used as pointers
class PlayerActionHandler;
class GameDisp;

class GameEngine {
private:
    Deck& deck;
    Dealer dealer;
    std::vector<Player> players;
    GameState currentState;
    int currentPlayerIndex;
    std::map<int, bool> playerHasSurrendered;
    
    std::unique_ptr<SplitHand> splitManager;
    
    PlayerActionHandler* actionHandler;
    GameDisp* display;
    
    Stats gameStats;

    std::unique_ptr<Counting> countingSystem;
    std::unique_ptr<BasicStrat> basicStrategy;

public:
    GameEngine(Deck& gameDeck);
    ~GameEngine() = default;  
    
    // Game Setup
    void addPlayer(const std::string& playerName);
    void startNewGame();
    void clearPlayers();
    
    // Game Flow methods
    void dealInitialCards();
    void playGame();
    void playPlayerTurn(int playerIndex);
    void playDealerTurn();
    void endGame();
    
    // Display methods
    void displayPlayerHand(const Player& player) const;
    void displayDealerHand(bool showHoleCard = false) const;
    void displayResults() const;
    
    // Game state queries
    GameState getCurrentState() const;
    bool isGameOver() const;
    int getCurrentPlayerIndex() const;
    bool isDeckReshuffleNeeded() const;
    
    // Player actions
    bool canPlayerSplit(int playerIndex);
    bool playerSplits(int playerIndex);
    bool playerHit(int playerIndex);
    bool playerStand(int playerIndex);
    bool playerDoublesDown(int playerIndex);
    bool playerSurrenders(int playerIndex);
    
    // Statistics methods
    void updateGameStats();
    void displayGameStats() const;
    void resetGameStats();
    
    // Getters for accessing components
    Stats* getGameStats() { return &gameStats; }
    const Stats* getGameStats() const { return &gameStats; }
    SplitHand* getSplitManager() const { return splitManager.get(); }
    
    void setActionHandler(PlayerActionHandler* handler) { actionHandler = handler; }
    void setDisplay(GameDisp* disp) { display = disp; }

    Counting* getCountingSystem() const { return countingSystem.get(); }
    BasicStrat* getBasicStrategy() const { return basicStrategy.get(); }
    void enableCounting(bool enabled);
    void enableBasicStrategy(bool enabled);
    void setCountingSystem(CountingSystem system);

    const Dealer& getDealer() const { return dealer; }
    const std::vector<Player>& getPlayers() const { return players; }
};

#endif