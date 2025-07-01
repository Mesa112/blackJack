#ifndef BLACKJACKGUI_H
#define BLACKJACKGUI_H

#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include <QGroupBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QDoubleSpinBox>

#include "GameEngine.h"
#include "deck.h"
#include "cardImg.h"
#include "Stats.h" 

#include <iostream>
#include <random>

class BlackjackGUI : public QMainWindow {
    Q_OBJECT

private:
    // Game components
    Deck* gameDeck;
    GameEngine* gameEngine;
    CardImageManager* cardImageManager;
    
    // GUI Components
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    QWidget* dealerCardWidget;
    QWidget* playerCardWidget;
    
    // Labels for displaying game info
    QLabel* dealerLabel;
    QLabel* playerLabel;
    QLabel* countLabel;
    QLabel* statsLabel;
    QLabel* statusLabel;
    
    // Card display layouts
    QHBoxLayout* dealerCardLayout;
    QHBoxLayout* playerCardLayout;
    
    // Game area
    QGroupBox* dealerArea;
    QGroupBox* playerArea;
    QGroupBox* controlArea;
    QGroupBox* statsArea;
    
    // Card display 
    QHBoxLayout* dealerCards;
    QHBoxLayout* playerCards;
    QLabel* dealerTotal;
    QLabel* playerTotal;
    
    // Action buttons
    QPushButton* hitButton;
    QPushButton* standButton;
    QPushButton* doubleButton;
    QPushButton* splitButton;
    QPushButton* surrenderButton;  
    QPushButton* newGameButton;
    QPushButton* insuranceButton;
    
    // Game info
    QTextEdit* gameLog;
    QLabel* deckInfo;
    QLabel* countInfo;
    QProgressBar* deckProgress;
    
    // Settings
    QCheckBox* enableCounting;
    QComboBox* countingSystem;
    QCheckBox* showStrategy;
    
    // Stats display
    QLabel* gamesPlayed;
    QLabel* winRate;
    QLabel* strategyAccuracy;
    
    // Animation components
    QSequentialAnimationGroup* dealAnimation;
    QTimer* animationTimer;
    std::vector<QLabel*> pendingCards;
    int currentAnimationIndex;
    
    // Hole card tracking
    QLabel* dealerHoleCardLabel;
    bool holeCardRevealed;
    
    // Betting system
    double playerBalance;
    double currentBet;
    double minimumBet;
    double maximumBet;
    
    // Betting controls
    QLabel* balanceLabel;
    QLabel* betLabel;
    QDoubleSpinBox* betAmountSpinBox; 
    QPushButton* placeBetButton;
    QPushButton* maxBetButton;
    QPushButton* clearBetButton;
    
    // Even money dialog
    bool evenMoneyOffered;
    bool isPlayingSplitHands;
    int currentSplitHandIndex;
    int totalSplitHands;

    // Offer Insurance
    bool insuranceOffered;
    double insuranceAmount;

public:
    BlackjackGUI(QWidget *parent = nullptr);
    ~BlackjackGUI();

private slots:
    void onHitClicked();
    void onStandClicked();
    void onDoubleClicked();
    void onNewGameClicked();
    void onSplitClicked(); 
    void onPlaceBet();
    void onMaxBet();
    void onClearBet();
    void onBetAmountChanged(double amount);
    void onInsuranceClicked();
    void onSurrenderClicked();

private:
    void setupUI();
    void updateDisplay();
    void updateVisualCards();
    void clearCardLayouts();
    void addCardToLayout(QHBoxLayout* layout, const Card& card);
    void addBackCardToLayout(QHBoxLayout* layout);
    void enableActions(bool enabled);
    void logMessage(const QString& message);
    
    // Helper methods for formatting display text
    QString formatDealerHand(bool showHoleCard) const;
    QString formatPlayerHand(int playerIndex) const;
    
    // Betting methods
    QGroupBox* setupBettingControls();  
    void updateBettingDisplay();
    void offerEvenMoney();
    void processBetResult(GameResult result, bool isBlackjack = false, bool doubledDown = false);
    void resetForNewBet();
    
    // Animation methods
    void dealCardsWithAnimation();
    void addCardToLayoutAnimated(QHBoxLayout* layout, const Card& card, int delayMs = 0);
    void addBackCardToLayoutAnimated(QHBoxLayout* layout, int delayMs = 0);
    void flipHoleCard(QLabel* cardLabel, const Card& revealedCard);
    void revealHoleCard();
    void addBackCardToLayoutAnimatedTracked(QHBoxLayout* layout, int delayMs = 0);
    
    // Game state methods
    void updateGameStats();
    void updateSplitButtonState();  
    void addCardToLayoutStatic(QHBoxLayout* layout, const Card& card);

    void handleSplitHandCompletion();
    void processRegularGameResult();
    void processSplitHandResults();

    void offerInsurance();
    void processInsuranceResult(bool dealerHasBlackjack);
    void updateGameButtonStates();

    void triggerCountingQuiz();
    void updateDeckStatus();
    bool checkMinimumFundsForActions();

    void endCurrentGame();
};

#endif