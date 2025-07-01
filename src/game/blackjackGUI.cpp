#include "blackjackGUI.h"
#include <iostream>
#include <QDir>

BlackjackGUI::BlackjackGUI(QWidget *parent)
    : QMainWindow(parent), gameDeck(nullptr), gameEngine(nullptr), cardImageManager(nullptr),
      dealerHoleCardLabel(nullptr), holeCardRevealed(false),
      playerBalance(0.0), currentBet(0.0), minimumBet(15.0), maximumBet(3000.0),
      evenMoneyOffered(false), isPlayingSplitHands(false), currentSplitHandIndex(0), 
      totalSplitHands(0), insuranceOffered(false), insuranceAmount(0.0) {
    
    gameDeck = new Deck();
    gameEngine = new GameEngine(*gameDeck);
    gameEngine->addPlayer("Player");

    QString cardImagePath = "/Users/estebanm/Desktop/Blackjack/CardImg/";
    cardImageManager = new CardImageManager(cardImagePath, 80, 112);

    setupUI();


    logMessage("🃏 NEW 8-DECK SHOE INITIALIZED");
    logMessage(QString("📚 Starting with %1 cards (8 decks × 52 cards)").arg(gameDeck->getCardsRemaining()));
    logMessage("🔄 Shuffle point: ~60-80 cards remaining");
    logMessage("Welcome to Blackjack! Click 'New Game' to start.");

    bool ok;
    double startingBalance = QInputDialog::getDouble(this, "Starting Balance", 
        "Enter your starting balance:", 1000.0, 100.0, 10000.0, 2, &ok);
    
    if (ok) {
        playerBalance = startingBalance;
    } else {
        playerBalance = 1000.0; // Default fallback
    }

}

BlackjackGUI::~BlackjackGUI() {
    delete cardImageManager;
    delete gameEngine;
    delete gameDeck;
}

void BlackjackGUI::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    setWindowTitle("Blackjack Game");
    setMinimumSize(1200, 800);

    QGroupBox* bettingArea = setupBettingControls();
    mainLayout->addWidget(bettingArea);
    
    // Style with responsive design
    setStyleSheet(R"(
        QMainWindow { background-color: #0e4b1c; }
        QLabel { color: white; font-size: 14px; }
        QPushButton { 
            background-color: #4CAF50; 
            border: none; 
            color: white; 
            padding: 12px 24px; 
            font-size: 14px; 
            border-radius: 6px; 
            margin: 3px;
            min-width: 120px;
        }
        QPushButton:hover { background-color: #45a049; }
        QPushButton:disabled { background-color: #888888; color: #cccccc; }
        QTextEdit { 
            background-color: white; 
            font-family: 'Monaco', 'Consolas', 'Courier New', monospace;
            font-size: 12px;
            border: 2px solid #666;
            border-radius: 5px;
            padding: 8px;
        }
        QGroupBox { 
            color: white; 
            font-weight: bold; 
            border: 2px solid #cccccc; 
            border-radius: 8px; 
            margin-top: 15px; 
            padding-top: 15px;
            font-size: 16px;
        }
        QGroupBox::title { 
            subcontrol-origin: margin; 
            left: 15px; 
            padding: 0 8px 0 8px; 
        }
    )");
    
    // Title
    QLabel* title = new QLabel("🃏 Professional Blackjack 🃏");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: gold; margin: 15px; padding: 10px;");
    mainLayout->addWidget(title);
    
    // Game area with proper layout
    QGroupBox* gameArea = new QGroupBox("Game Table");
    QVBoxLayout* gameLayout = new QVBoxLayout(gameArea);
    gameLayout->setSpacing(20);
    
    // Dealer area
    QGroupBox* dealerArea = new QGroupBox("🎰 Dealer");
    QVBoxLayout* dealerMainLayout = new QVBoxLayout(dealerArea);
    
    dealerLabel = new QLabel("Dealer: [Waiting for new game]");
    dealerLabel->setStyleSheet("font-size: 18px; padding: 10px; color: white; font-weight: bold;");
    dealerLabel->setAlignment(Qt::AlignCenter);
    
    // Setup dealer card area
    dealerCardWidget = new QWidget();
    dealerCardWidget->setMinimumHeight(130);
    dealerCardWidget->setMaximumHeight(150);
    dealerCardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dealerCardLayout = new QHBoxLayout(dealerCardWidget);
    dealerCardLayout->setAlignment(Qt::AlignCenter);
    dealerCardLayout->setSpacing(5); 
    dealerCardLayout->setContentsMargins(10, 5, 10, 5);
    
    dealerMainLayout->addWidget(dealerLabel);
    dealerMainLayout->addWidget(dealerCardWidget);
    dealerMainLayout->addStretch();
    
    // Player area
    QGroupBox* playerArea = new QGroupBox("🎮 Player");
    QVBoxLayout* playerMainLayout = new QVBoxLayout(playerArea);
    
    playerLabel = new QLabel("Player: [Waiting for new game]");
    playerLabel->setStyleSheet("font-size: 18px; padding: 10px; color: white; font-weight: bold;");
    playerLabel->setAlignment(Qt::AlignCenter);
    
    // Setup player card area
    playerCardWidget = new QWidget();
    playerCardWidget->setMinimumHeight(130);
    playerCardWidget->setMaximumHeight(150);
    playerCardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    playerCardLayout = new QHBoxLayout(playerCardWidget);
    playerCardLayout->setAlignment(Qt::AlignCenter);
    playerCardLayout->setSpacing(5);
    playerCardLayout->setContentsMargins(10, 5, 10, 5);
    
    playerMainLayout->addWidget(playerLabel);
    playerMainLayout->addWidget(playerCardWidget);
    playerMainLayout->addStretch();
    
    gameLayout->addWidget(dealerArea);
    gameLayout->addWidget(playerArea);
    mainLayout->addWidget(gameArea);
    
    // Action buttons
    QGroupBox* actionArea = new QGroupBox("Actions");
    QHBoxLayout* buttonLayout = new QHBoxLayout(actionArea);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(15);
    
    newGameButton = new QPushButton("🎲 New Game");
    hitButton = new QPushButton("🃏 Hit");
    standButton = new QPushButton("✋ Stand");
    doubleButton = new QPushButton("💰 Double Down");
    splitButton = new QPushButton("✂️ Split");
    surrenderButton = new QPushButton("🏳️ Surrender");
    insuranceButton = new QPushButton("🛡️ Insurance");
    
    // Initially disable game actions
    hitButton->setEnabled(false);
    standButton->setEnabled(false);
    doubleButton->setEnabled(false);
    splitButton->setEnabled(false);
    surrenderButton->setEnabled(false); 
    insuranceButton->setEnabled(false);

    splitButton->setStyleSheet(
        "QPushButton:enabled { background-color: #2ecc71; color: white; }"  // Green when enabled
        "QPushButton:disabled { background-color: #7f8c8d; color: #bdc3c7; }" // Gray when disabled
    );

    surrenderButton->setStyleSheet(  
        "QPushButton:enabled { background-color: #e67e22; color: white; font-weight: bold; }"
        "QPushButton:disabled { background-color: #7f8c8d; color: #bdc3c7; }"
    );

    insuranceButton->setStyleSheet(
        "QPushButton:enabled { background-color: #f39c12; color: white; font-weight: bold; }"
        "QPushButton:disabled { background-color: #7f8c8d; color: #bdc3c7; }"
    );
    
    buttonLayout->addWidget(newGameButton);
    buttonLayout->addWidget(hitButton);
    buttonLayout->addWidget(standButton);
    buttonLayout->addWidget(doubleButton);
    buttonLayout->addWidget(splitButton);
    buttonLayout->addWidget(surrenderButton);
    buttonLayout->addWidget(insuranceButton);
    
    mainLayout->addWidget(actionArea);
    
    // Info area with better layout
    QHBoxLayout* infoLayout = new QHBoxLayout();
    infoLayout->setSpacing(15);
    
    // Game log
    QGroupBox* logArea = new QGroupBox("Game Log");
    QVBoxLayout* logLayout = new QVBoxLayout(logArea);
    gameLog = new QTextEdit();
    gameLog->setMaximumHeight(180);
    gameLog->setMinimumHeight(120);
    gameLog->setReadOnly(true);
    logLayout->addWidget(gameLog);
    
    // Stats with better formatting
    QGroupBox* statsArea = new QGroupBox("Game Info");
    QVBoxLayout* statsLayout = new QVBoxLayout(statsArea);
    statsLayout->setSpacing(8);
    
    countLabel = new QLabel("💳 Count: Disabled");
    countLabel->setStyleSheet("font-size: 14px; padding: 5px; color: #ffff88;");

    QPushButton* toggleCountingButton = new QPushButton("Enable Counting");
    toggleCountingButton->setStyleSheet("background-color: #9b59b6; color: white; padding: 5px; font-size: 12px;");
    connect(toggleCountingButton, &QPushButton::clicked, [this, toggleCountingButton]() {
        if (gameEngine && gameEngine->getCountingSystem()) {
            auto countingSystem = gameEngine->getCountingSystem();
            bool isEnabled = countingSystem->isCountingEnabled();
            
            if (isEnabled) {
                countingSystem->enableCounting(false);
                toggleCountingButton->setText("Enable Counting");
                countLabel->setText("💳 Count: Disabled");
                logMessage("🚫 Card counting disabled");
            } else {
                countingSystem->enableCounting(true);
                toggleCountingButton->setText("Disable Counting");
                countLabel->setText("💳 Count: Enabled - Running: 0 | True: 0.0");
                logMessage("✅ Card counting enabled with Hi-Lo system");
                logMessage("📚 You may receive random counting quizzes during play!");
            }
        }
    });
    
    statsLabel = new QLabel("📊 Games: 0 | Wins: 0 | Win Rate: 0%");
    statsLabel->setStyleSheet("font-size: 14px; padding: 5px; color: #88ff88;");
    
    statusLabel = new QLabel("🎯 Status: Ready to play");
    statusLabel->setStyleSheet("font-size: 14px; padding: 5px; color: #88ddff;");
    
    statsLayout->addWidget(countLabel);
    statsLayout->addWidget(toggleCountingButton);
    statsLayout->addWidget(statsLabel);
    statsLayout->addWidget(statusLabel);
    statsLayout->addStretch();
    
    infoLayout->addWidget(logArea, 3);  // 3/4 of the width
    infoLayout->addWidget(statsArea, 1); // 1/4 of the width
    
    QWidget* infoWidget = new QWidget();
    infoWidget->setLayout(infoLayout);
    mainLayout->addWidget(infoWidget);
    
    // Connect signals
    connect(newGameButton, &QPushButton::clicked, this, &BlackjackGUI::onNewGameClicked);
    connect(hitButton, &QPushButton::clicked, this, &BlackjackGUI::onHitClicked);
    connect(standButton, &QPushButton::clicked, this, &BlackjackGUI::onStandClicked);
    connect(doubleButton, &QPushButton::clicked, this, &BlackjackGUI::onDoubleClicked);
    connect(splitButton, &QPushButton::clicked, this, &BlackjackGUI::onSplitClicked);
    connect(surrenderButton, &QPushButton::clicked, this, &BlackjackGUI::onSurrenderClicked);
    connect(insuranceButton, &QPushButton::clicked, this, &BlackjackGUI::onInsuranceClicked);
}

void BlackjackGUI::onNewGameClicked() {
    // Check if player has enough money to play
    if (playerBalance + currentBet < minimumBet) { 
        QMessageBox::warning(this, "Insufficient Funds", 
            QString("You need at least $%1 to play!\nYour total funds: $%2")
            .arg(minimumBet, 0, 'f', 2)
            .arg(playerBalance + currentBet, 0, 'f', 2));
        return;
    }
    
    // Check if bet is placed
    if (currentBet < minimumBet) {
        QMessageBox::information(this, "Place Your Bet", "Bet must be at or above minimum");
        return;
    }
    
    logMessage("=== NEW GAME STARTED ===");
    logMessage(QString("💰 Playing with bet: $%1").arg(currentBet, 0, 'f', 2));

    int cardsRemaining = gameDeck->getCardsRemaining();
    logMessage(QString("🃏 Cards remaining in shoe: %1").arg(cardsRemaining));
    
    // Check for reshuffle BEFORE dealing
    if (gameDeck->isThresholdReached()) {
        logMessage("🔄 CUT CARD REACHED! Reshuffling 8-deck shoe...");
        gameDeck->resetDeck();  // This reshuffles
        logMessage(QString("✨ Fresh shoe! New 8-deck shoe with %1 cards").arg(gameDeck->getCardsRemaining()));
    }

    updateGameButtonStates();
    
    // Start new game
    gameEngine->startNewGame();
    gameEngine->dealInitialCards();
    
    evenMoneyOffered = false;
    insuranceOffered = false;
    insuranceAmount = 0.0;
    updateDisplay();
    
    // Check for dealer showing Ace (even money opportunity)
    const Dealer& dealer = gameEngine->getDealer();
    if (dealer.getCardCount() > 0 && dealer.getCard(0).getValue() == 1) { // Dealer shows Ace
        const std::vector<Player>& players = gameEngine->getPlayers();
        if (!players.empty() && players[0].isBlackjack()) {
            // Player has blackjack and dealer shows Ace - offer even money
            offerEvenMoney();
            return;
        }else{
            offerInsurance();
            return;
        }
    }
    
    // Check for player blackjack (normal case)
    const std::vector<Player>& players = gameEngine->getPlayers();
    if (!players.empty() && players[0].isBlackjack()) {
        logMessage("🎉 BLACKJACK! You got 21 with 2 cards!");
        gameEngine->playDealerTurn();
        updateDisplay();
        
        // Check if dealer also has blackjack
        if (dealer.checkForBlackjack()) {
            logMessage("Dealer also has blackjack - it's a PUSH!");
            statusLabel->setText("🎯 Status: Push! Both have blackjack.");
            processBetResult(GameResult::PUSH, true);
        } else {
            logMessage("🏆 YOU WIN with blackjack!");
            statusLabel->setText("🎯 Status: BLACKJACK WIN! 🎉");
            processBetResult(GameResult::BLACKJACK, true);
        }

        enableActions(false);
        endCurrentGame();
        resetForNewBet();
        return;
    }
    
    // Check for dealer blackjack (without player blackjack)
    if (dealer.checkForBlackjack()) {
        logMessage("💥 Dealer has blackjack! You lose.");
        statusLabel->setText("🎯 Status: Dealer blackjack - You lose!");
        processBetResult(GameResult::LOSS);
        gameEngine->playDealerTurn();
        updateDisplay();
        enableActions(false);
        endCurrentGame();
        resetForNewBet();
        return;
    }
    
    enableActions(true);
    placeBetButton->setEnabled(false); 
    updateSplitButtonState(); 
    statusLabel->setText("🎯 Status: Your turn - choose an action!");
}


void BlackjackGUI::offerEvenMoney() {
    evenMoneyOffered = true;
    
    QMessageBox evenMoneyDialog;
    evenMoneyDialog.setWindowTitle("Even Money Offer");
    evenMoneyDialog.setText("🎰 EVEN MONEY OFFER 🎰");
    evenMoneyDialog.setInformativeText(
        QString("You have blackjack and the dealer is showing an Ace!\n\n"
                "Take even money? You'll win $%1 guaranteed.\n"
                "Or risk it for $%2 if dealer doesn't have blackjack?\n\n"
                "Current bet: $%3")
        .arg(currentBet, 0, 'f', 2)           // Even money payout (1:1)
        .arg(currentBet * 1.5, 0, 'f', 2)    // Blackjack payout (3:2)
        .arg(currentBet, 0, 'f', 2));
    
    evenMoneyDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    evenMoneyDialog.setDefaultButton(QMessageBox::No);
    evenMoneyDialog.button(QMessageBox::Yes)->setText("Take Even Money ($" + QString::number(currentBet, 'f', 2) + ")");
    evenMoneyDialog.button(QMessageBox::No)->setText("Risk It for Blackjack Payout");
    
    int choice = evenMoneyDialog.exec();
    
    if (choice == QMessageBox::Yes) {
        // Player takes even money
        logMessage("💰 Even money accepted! You win $" + QString::number(currentBet, 'f', 2));
        statusLabel->setText("🎯 Status: Even money taken - guaranteed win!");
        
        // Pay even money (1:1)
        playerBalance += currentBet * 2; // Original bet + even money win
        updateBettingDisplay();
        
        // Game ends, reveal dealer cards
        gameEngine->playDealerTurn();
        updateDisplay();
        
        const Dealer& dealer = gameEngine->getDealer();
        if (dealer.isBlackjack()) {
            logMessage("✅ Good choice! Dealer had blackjack - you avoided a push!");
        } else {
            logMessage("😔 Dealer didn't have blackjack - you could have won more!");
        }
        
        enableActions(false);
        resetForNewBet();
    } else {
        // Player risks it
        logMessage("🎲 Even money declined - going for blackjack payout!");
        
        // Check if dealer has blackjack
        const Dealer& dealer = gameEngine->getDealer();
        if (dealer.checkForBlackjack()) {
            logMessage("💔 Dealer has blackjack - PUSH! (You should have taken even money!)");
            statusLabel->setText("🎯 Status: Push! Should have taken even money...");
            processBetResult(GameResult::PUSH, true);
        } else {
            logMessage("🎊 Dealer doesn't have blackjack - BLACKJACK WIN!");
            statusLabel->setText("🎯 Status: BLACKJACK WIN! Good risk!");
            processBetResult(GameResult::BLACKJACK, true);
        }
        
        gameEngine->playDealerTurn();
        updateDisplay();
        enableActions(false);
        resetForNewBet();
    }
}


QGroupBox* BlackjackGUI::setupBettingControls() {
    QGroupBox* bettingArea = new QGroupBox("💰 Betting Controls");
    QHBoxLayout* bettingLayout = new QHBoxLayout(bettingArea);
    
    // Balance display
    balanceLabel = new QLabel(QString("💳 Balance: $%1").arg(playerBalance, 0, 'f', 2));
    balanceLabel->setStyleSheet("color: #00ff00; font-size: 16px; font-weight: bold; padding: 5px;");
    
    // Current bet display
    betLabel = new QLabel("🎯 Current Bet: $0.00");
    betLabel->setStyleSheet("color: #ffff00; font-size: 16px; font-weight: bold; padding: 5px;");
    
    // Bet amount selector
    QLabel* betAmountLabel = new QLabel("Bet Amount:");
    betAmountLabel->setStyleSheet("color: white; font-size: 14px;");
    
    betAmountSpinBox = new QDoubleSpinBox();
    betAmountSpinBox->setRange(static_cast<int>(minimumBet), static_cast<int>(maximumBet)); // Use proper range
    betAmountSpinBox->setValue(static_cast<int>(minimumBet));
    betAmountSpinBox->setSingleStep(0.5); 
    betAmountSpinBox->setDecimals(2);
    betAmountSpinBox->setPrefix("$"); 
    betAmountSpinBox->setStyleSheet("background: white; padding: 5px; font-size: 14px; min-width: 100px;");
    
    // Betting buttons
    placeBetButton = new QPushButton("💰 Place Bet");
    maxBetButton = new QPushButton("🎰 Max Bet");
    clearBetButton = new QPushButton("🚫 Clear Bet");
    
    placeBetButton->setStyleSheet("background-color: #2ecc71; min-width: 100px;");
    maxBetButton->setStyleSheet("background-color: #e74c3c; min-width: 100px;");
    clearBetButton->setStyleSheet("background-color: #95a5a6; min-width: 100px;");
    
    // Layout
    bettingLayout->addWidget(balanceLabel);
    bettingLayout->addWidget(betLabel);
    bettingLayout->addWidget(betAmountLabel);
    bettingLayout->addWidget(betAmountSpinBox);
    bettingLayout->addWidget(placeBetButton);
    bettingLayout->addWidget(maxBetButton);
    bettingLayout->addWidget(clearBetButton);
    bettingLayout->addStretch();
    
    // Connect signals
    connect(placeBetButton, &QPushButton::clicked, this, &BlackjackGUI::onPlaceBet);
    connect(maxBetButton, &QPushButton::clicked, this, &BlackjackGUI::onMaxBet);
    connect(clearBetButton, &QPushButton::clicked, this, &BlackjackGUI::onClearBet);
    connect(betAmountSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            this, &BlackjackGUI::onBetAmountChanged);
    
    return bettingArea;
}

// Betting control methods
void BlackjackGUI::onPlaceBet() {
    double betAmount = betAmountSpinBox->value();
    
    if (betAmount > playerBalance) {
        QMessageBox::warning(this, "Insufficient Funds", 
            QString("You only have $%1 available!").arg(playerBalance, 0, 'f', 2));
        return;
    }
    
    if (betAmount < minimumBet) {
        QMessageBox::warning(this, "Bet Too Low", 
            QString("Minimum bet is $%1!").arg(minimumBet, 0, 'f', 2));
        return;
    }

    bool isAllIn = (betAmount >= playerBalance);

    if (isAllIn) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, 
            "All-In Warning", 
            QString("⚠️ You're betting your entire balance of $%1!\n\n"
                   "If you lose, you won't be able to place another bet.\n"
                   "Are you sure you want to go all-in?")
            .arg(playerBalance, 0, 'f', 2),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);
        
        if (reply != QMessageBox::Yes) {
            return;
        }
        
        logMessage("🎰 GOING ALL-IN! GOOD LUCK!!!");
    }

    playerBalance -= betAmount;    
    currentBet = betAmount;
    updateBettingDisplay();
    logMessage(QString("💰 Bet placed: $%1").arg(currentBet, 0, 'f', 2));

    if (isAllIn) {
        logMessage("⚠️ ALL-IN: No money left for doubles or splits!");
    }
    
    // Enable new game button when bet is placed
    placeBetButton->setEnabled(false);
    updateGameButtonStates();
}

void BlackjackGUI::onMaxBet() {
    double maxAffordable = std::min(maximumBet, playerBalance);
    betAmountSpinBox->setValue(maxAffordable);
    
    playerBalance -= maxAffordable;
    currentBet = maxAffordable;
    updateBettingDisplay();
    logMessage(QString("💰 Max bet placed: $%1").arg(currentBet, 0, 'f', 2));
    
    newGameButton->setEnabled(true);
    placeBetButton->setEnabled(false);
    updateGameButtonStates();
}


void BlackjackGUI::onHitClicked() {
    logMessage("Player chooses to HIT");
    
    bool success = gameEngine->playerHit(0);
    updateDisplay();
    
    if (!success) {
        logMessage("💥 BUST! Hand busted!");
        
        if (isPlayingSplitHands) {
            // Move to next split hand or finish split play
            handleSplitHandCompletion();
        } else {
            // Regular game - player busted
            statusLabel->setText("🎯 Status: BUSTED!");
            enableActions(false);
            processBetResult(GameResult::LOSS);
            gameEngine->playDealerTurn();
            updateDisplay();
            endCurrentGame();
            resetForNewBet();
        }
    }
}


void BlackjackGUI::onStandClicked() {
    logMessage("Player chooses to STAND");
    
    gameEngine->playerStand(0);
    
    if (isPlayingSplitHands) {
        // Move to next split hand or finish split play
        handleSplitHandCompletion();
    } else {
        // Regular game flow
        enableActions(false);
        statusLabel->setText("🎯 Status: Dealer's turn...");
        
        gameEngine->playDealerTurn();
        updateDisplay();
        
        // Process results for regular game
        processRegularGameResult();
        endCurrentGame();
        resetForNewBet();
    }
}

void BlackjackGUI::onDoubleClicked() {
    // Check if player has enough money for additional bet
    if (playerBalance < currentBet) {
        QMessageBox::warning(this, "Insufficient Funds", 
            QString("You need $%1 more to double down!\nYour balance: $%2")
            .arg(currentBet, 0, 'f', 2)
            .arg(playerBalance, 0, 'f', 2));
        return;
    }
    
    logMessage("Player chooses to DOUBLE DOWN");
    logMessage(QString("💰 Original bet: $%1").arg(currentBet, 0, 'f', 2));
    
    // *** DEDUCT ADDITIONAL BET FOR DOUBLE DOWN ***
    playerBalance -= currentBet; // This is the additional bet
    updateBettingDisplay();
    logMessage(QString("💸 Additional bet for double down: -$%1").arg(currentBet, 0, 'f', 2));
    logMessage(QString("💰 Total at risk: $%1 (doubled)").arg(currentBet * 2, 0, 'f', 2));
    
    bool success = gameEngine->playerDoublesDown(0);
    updateDisplay();
    
    if (!success) {
        logMessage("💥 BUST after doubling down!");
        
        if (isPlayingSplitHands) {
            handleSplitHandCompletion();
        } else {
            statusLabel->setText("🎯 Status: BUSTED!");
            enableActions(false);
            //  Pass doubledDown=true and ensure stats record correctly
            processBetResult(GameResult::LOSS, false, true);
            gameEngine->playDealerTurn();
            updateDisplay();
            resetForNewBet();
        }
    } else {
        if (isPlayingSplitHands) {
            logMessage(QString("Doubled down on split hand %1").arg(currentSplitHandIndex + 1));
            handleSplitHandCompletion();
        } else {
            statusLabel->setText("🎯 Status: Doubled down - Dealer's turn...");
            enableActions(false);
            gameEngine->playDealerTurn();
            updateDisplay();
            
            //  Process results for regular doubled hand with correct math
            const std::vector<Player>& players = gameEngine->getPlayers();
            const Dealer& dealer = gameEngine->getDealer();
            
            if (players[0].getTotalValue() > dealer.getTotalValue() || dealer.isBusted()) {
                logMessage("🎉 Double down wins!");
                processBetResult(GameResult::WIN, false, true);
            } else if (players[0].getTotalValue() < dealer.getTotalValue()) {
                logMessage("💸 Double down loses!");
                processBetResult(GameResult::LOSS, false, true);
            } else {
                logMessage("🤝 Double down pushes!");
                processBetResult(GameResult::PUSH, false, true);
            }
            
            resetForNewBet();
        }
    }
}

void BlackjackGUI::onInsuranceClicked() {
    if (!insuranceOffered) return;
    
    // Check if player has enough money for insurance
    if (playerBalance < insuranceAmount) {
        QMessageBox::warning(this, "Insufficient Funds", 
            QString("You need $%1 for insurance!\nYour balance: $%2")
            .arg(insuranceAmount, 0, 'f', 2)
            .arg(playerBalance, 0, 'f', 2));
        return;
    }
    
    // Deduct insurance amount
    playerBalance -= insuranceAmount;
    updateBettingDisplay();
    
    logMessage(QString("💸 Insurance purchased: -$%1").arg(insuranceAmount, 0, 'f', 2));
    
    // Disable insurance button
    insuranceButton->setEnabled(false);
    insuranceButton->setText("🛡️ Insurance (Taken)");
    
    // Check if dealer has blackjack
    const Dealer& dealer = gameEngine->getDealer();
    bool dealerHasBlackjack = dealer.checkForBlackjack();
    
    if (dealerHasBlackjack) {
        logMessage("✅ Insurance pays! Dealer has blackjack!");
        logMessage(QString("💰 Insurance payout: +$%1").arg(insuranceAmount * 2, 0, 'f', 2));
        
        // Pay insurance (2:1)
        playerBalance += insuranceAmount * 3; // Return bet + 2:1 payout
        updateBettingDisplay();
        
        // Reveal dealer cards and end game
        gameEngine->playDealerTurn();
        updateDisplay();
        
        // Process main bet result
        const std::vector<Player>& players = gameEngine->getPlayers();
        if (!players.empty() && players[0].isBlackjack()) {
            logMessage("🤝 Main bet pushes (both have blackjack)");
            statusLabel->setText("🎯 Status: Insurance won, main bet pushes!");
            processBetResult(GameResult::PUSH, true);
        } else {
            logMessage("💸 Main bet loses to dealer blackjack");
            statusLabel->setText("🎯 Status: Insurance won, main bet loses!");
            processBetResult(GameResult::LOSS);
        }
        
        enableActions(false);
        resetForNewBet();
    } else {
        logMessage("❌ Insurance loses - dealer doesn't have blackjack");
        statusLabel->setText("🎯 Status: Insurance lost - continue playing!");
        
        // Continue with normal game
        // Player can still play their hand normally
    }
    
    insuranceOffered = false;
}

void BlackjackGUI::onSurrenderClicked() {
    if (!gameEngine) return;
    
    logMessage("Player chooses to SURRENDER");
    
    // Surrender gives back half the bet
    double surrenderReturn = currentBet / 2.0;
    playerBalance += surrenderReturn;
    updateBettingDisplay();
    
    logMessage(QString("🏳️ Surrendered! Lost half bet: -$%1, returned: +$%2")
               .arg(currentBet / 2.0, 0, 'f', 2)
               .arg(surrenderReturn, 0, 'f', 2));
    
    statusLabel->setText("🏳️ Status: Surrendered - Lost half bet");
    
    // Record as loss in stats
    if (gameEngine && gameEngine->getGameStats()) {
        gameEngine->getGameStats()->updatePlayerStats("Player", GameResult::LOSS);
        updateGameStats();
    }
    
    // End the game
    enableActions(false);
    endCurrentGame();
    resetForNewBet();
}

void BlackjackGUI::updateGameButtonStates() {
    // New game should be disabled when:
    // 1. A bet is placed AND cards are dealt (game in progress)
    // 2. No bet is placed (can't start without bet)
    
    bool betPlaced = (currentBet > 0);
    bool gameInProgress = (gameEngine && 
                          gameEngine->getCurrentState() != GameState::SETUP &&
                          gameEngine->getCurrentState() != GameState::GAME_OVER);
    
    // Enable new game only if bet is placed AND no game in progress
    bool enableNewGame = betPlaced && !gameInProgress;
    
    newGameButton->setEnabled(enableNewGame);
    
    if (gameInProgress) {
        newGameButton->setStyleSheet("background-color: #888888; color: #cccccc;");
        newGameButton->setText("🎲 Game in Progress");
    } else if (!betPlaced) {
        newGameButton->setStyleSheet("background-color: #888888; color: #cccccc;");
        newGameButton->setText("🎲 Place Bet First");
    } else {
        newGameButton->setStyleSheet("background-color: #4CAF50; color: white;");
        newGameButton->setText("🎲 New Game");
    }
}

void BlackjackGUI::updateDisplay() {
    // Update text display
    QString dealerText = formatDealerHand(gameEngine->getCurrentState() == GameState::GAME_OVER);
    QString playerText = formatPlayerHand(0);
    
    dealerLabel->setText(dealerText);
    playerLabel->setText(playerText);
    
    // Update visual cards
    updateVisualCards();
    
    // Update split button state
    updateSplitButtonState();
    
    // Update counting info
    if (gameEngine->getCountingSystem() && gameEngine->getCountingSystem()->isCountingEnabled()) {
        int running = gameEngine->getCountingSystem()->getRunningCount();
        double trueCount = gameEngine->getCountingSystem()->getTrueCount();
        countLabel->setText(QString("💳 Count: Running %1 | True %2").arg(running).arg(trueCount, 0, 'f', 1));
        
        // RANDOM QUIZ TRIGGER 
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0, 1.0);
        
        if (dis(gen) < 0.05) {  
            QTimer::singleShot(500, [this]() { 
                triggerCountingQuiz();
            });
        }
    } else {
        countLabel->setText("💳 Count: Disabled");
    }
    
    // Update game statistics
    updateGameStats();
}

void BlackjackGUI::triggerCountingQuiz() {
    if (!gameEngine || !gameEngine->getCountingSystem() || !gameEngine->getCountingSystem()->isCountingEnabled()) {
        return;
    }
    
    auto countingSystem = gameEngine->getCountingSystem();
    int actualRunning = countingSystem->getRunningCount();
    
    bool ok;
    int playerAnswer = QInputDialog::getInt(this, 
        "🎓 Counting Quiz", 
        "What is the current running count?", 
        0, -50, 50, 1, &ok);
    
    if (ok) {
        if (playerAnswer == actualRunning) {
            logMessage(QString("✅ Correct! Running count is %1").arg(actualRunning));
            QMessageBox::information(this, "Correct!", 
                QString("Well done! The running count is indeed %1").arg(actualRunning));
        } else {
            logMessage(QString("❌ Incorrect! You said %1, actual is %2").arg(playerAnswer).arg(actualRunning));
            QMessageBox::information(this, "Incorrect", 
                QString("Not quite. You said %1 but the running count is %2").arg(playerAnswer).arg(actualRunning));
        }
    }
}

void BlackjackGUI::updateGameStats() {
    if (!gameEngine || !gameEngine->getGameStats()) {
        statsLabel->setText("📊 Games: 0 | Wins: 0 | Win Rate: 0%");
        return;
    }
    
    const Stats* stats = gameEngine->getGameStats();
    
    // Get stats for the current player (should be "Player")
    std::string playerName = "Player";  // This matches what we add in constructor
    
    int wins = stats->getPlayerWins(playerName);
    int losses = stats->getPlayerLosses(playerName);
    int pushes = stats->getPlayerPushes(playerName);
    
    double winRate = 0.0;
    int totalHands = wins + losses + pushes;
    if (totalHands > 0) {
        winRate = (static_cast<double>(wins) / totalHands) * 100.0;
    }
    
    statsLabel->setText(QString("📊 Games: %1 | Wins: %2 | Win Rate: %3%")
        .arg(totalHands)
        .arg(wins)
        .arg(winRate, 0, 'f', 1));
    
    // Debug output to check if stats are being recorded
    if (totalHands > 0) {
        logMessage(QString("DEBUG: Player stats - W:%1 L:%2 P:%3").arg(wins).arg(losses).arg(pushes));
    }
}

void BlackjackGUI::revealHoleCard() {
    if (!dealerHoleCardLabel) return;
    
    const Dealer& dealer = gameEngine->getDealer();
    if (dealer.getCardCount() > 1) {
        // Get the actual hole card
        Card holeCard = dealer.getCard(1);
        
        flipHoleCard(dealerHoleCardLabel, holeCard);
        
        // Add any additional dealer cards that were dealt (hit cards)
        if (dealer.getCardCount() > 2) {
            // Remove the stretch temporarily
            QTimer::singleShot(550, [this, &dealer]() {
            QLayoutItem* stretch = dealerCardLayout->takeAt(dealerCardLayout->count() - 1);
            if (stretch && stretch->spacerItem()) {
                delete stretch;
            }
            
            // Add new cards with simple fade-in
            for (size_t i = 2; i < dealer.getCardCount(); ++i) {
                QLabel* newCardLabel = cardImageManager->createCardLabel(dealer.getCard(i), dealerCardWidget);
                dealerCardLayout->addWidget(newCardLabel);
                
                // Simple fade-in for additional cards
                QGraphicsOpacityEffect* opacity = new QGraphicsOpacityEffect(newCardLabel);
                newCardLabel->setGraphicsEffect(opacity);
                
                QPropertyAnimation* fadeAnimation = new QPropertyAnimation(opacity, "opacity");
                fadeAnimation->setDuration(400);
                fadeAnimation->setStartValue(0.0);
                fadeAnimation->setEndValue(1.0);
                fadeAnimation->start();
                
                connect(fadeAnimation, &QAbstractAnimation::finished, fadeAnimation, &QObject::deleteLater);
            }
            
            // Add stretch back
            dealerCardLayout->addStretch();
        });
        }
    }
}

void BlackjackGUI::updateVisualCards() {
    const Dealer& dealer = gameEngine->getDealer();
    
    // Check if dealer's hole card should be revealed
    if (dealer.isHoleCardRevealed() && !holeCardRevealed && dealerHoleCardLabel) {
        // Reveal the hole card with flip animation
        revealHoleCard();
        holeCardRevealed = true;
    } else if (!dealer.isHoleCardRevealed() || dealerHoleCardLabel == nullptr) {
        // Redraw all cards (normal case or first time)
        dealCardsWithAnimation();
        holeCardRevealed = dealer.isHoleCardRevealed();
    }
    
    // Update player cards if they've changed
    const std::vector<Player>& players = gameEngine->getPlayers();
    if (!players.empty()) {
        const Player& player = players[0];
        
        // Check if player has more cards than currently displayed
        int currentPlayerCards = 0;
        for (int i = 0; i < playerCardLayout->count(); ++i) {
            if (playerCardLayout->itemAt(i)->widget()) {
                currentPlayerCards++;
            }
        }
        
        // If player has new cards, add them with animation
        if (static_cast<size_t>(currentPlayerCards) < player.getCardCount()) {
            // Remove stretch
            QLayoutItem* stretch = playerCardLayout->takeAt(playerCardLayout->count() - 1);
            if (stretch && stretch->spacerItem()) {
                delete stretch;
            }
            
            // Add new cards
            for (size_t i = currentPlayerCards; i < player.getCardCount(); ++i) {
                addCardToLayoutAnimated(playerCardLayout, player.getCard(i), 0);
            }
        }
    }
}

void BlackjackGUI::clearCardLayouts() {
    // Clear dealer cards
    QLayoutItem* item;
    while ((item = dealerCardLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    // Clear player cards
    while ((item = playerCardLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void BlackjackGUI::dealCardsWithAnimation() {
    clearCardLayouts();
    
    const Dealer& dealer = gameEngine->getDealer();
    const std::vector<Player>& players = gameEngine->getPlayers();
    
    // Reset hole card tracking
    dealerHoleCardLabel = nullptr;
    holeCardRevealed = dealer.isHoleCardRevealed();
    
    // Deal player cards first (no animation delays for initial deal)
    if (!players.empty() && players[0].getCardCount() > 0) {
        const Player& player = players[0];
        for (size_t i = 0; i < player.getCardCount(); ++i) {
            addCardToLayout(playerCardLayout, player.getCard(i));
        }
    }
    
    // Deal dealer cards
    if (dealer.getCardCount() > 0) {
        // Dealer's first card (up card)
        addCardToLayout(dealerCardLayout, dealer.getCard(0));
        
        // Dealer's second card (hole card or revealed)
        if (dealer.getCardCount() > 1) {
            if (dealer.isHoleCardRevealed()) {
                // Show all remaining cards
                for (size_t i = 1; i < dealer.getCardCount(); ++i) {
                    addCardToLayout(dealerCardLayout, dealer.getCard(i));
                }
            } else {
                // Show back card and remember it for flipping later
                dealerHoleCardLabel = cardImageManager->createBackCardLabel(dealerCardWidget);
                dealerCardLayout->addWidget(dealerHoleCardLabel);
            }
        }
    }
    
    // Add stretch to center cards
    dealerCardLayout->addStretch();
    playerCardLayout->addStretch();
    
    // Force layout update
    dealerCardWidget->updateGeometry();
    playerCardWidget->updateGeometry();
    update();
}

void BlackjackGUI::addCardToLayoutStatic(QHBoxLayout* layout, const Card& card) {
    QWidget* parentWidget = (layout == dealerCardLayout) ? dealerCardWidget : playerCardWidget;
    QLabel* cardLabel = cardImageManager->createCardLabel(card, parentWidget);
    layout->addWidget(cardLabel);
}

void BlackjackGUI::addCardToLayout(QHBoxLayout* layout, const Card& card) {
    addCardToLayoutStatic(layout, card);
}

void BlackjackGUI::addBackCardToLayout(QHBoxLayout* layout) {
    QWidget* parentWidget = (layout == dealerCardLayout) ? dealerCardWidget : playerCardWidget;
    QLabel* backLabel = cardImageManager->createBackCardLabel(parentWidget);
    layout->addWidget(backLabel);
}

void BlackjackGUI::addBackCardToLayoutAnimatedTracked(QHBoxLayout* layout, int delayMs) {
    dealerHoleCardLabel = cardImageManager->createBackCardLabel();
    layout->addWidget(dealerHoleCardLabel);
    
    // Start off-screen and animate in
    QPoint finalPosition = dealerHoleCardLabel->pos();
    dealerHoleCardLabel->move(dealerHoleCardLabel->x() + 200, dealerHoleCardLabel->y());
    
    QPropertyAnimation* slideAnimation = new QPropertyAnimation(dealerHoleCardLabel, "pos");
    slideAnimation->setDuration(500);
    slideAnimation->setStartValue(QPoint(dealerHoleCardLabel->x(), dealerHoleCardLabel->y()));
    slideAnimation->setEndValue(finalPosition);
    slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    if (delayMs > 0) {
        QTimer::singleShot(delayMs, [slideAnimation]() {
            slideAnimation->start();
        });
    } else {
        slideAnimation->start();
    }
    
    connect(slideAnimation, &QAbstractAnimation::finished, slideAnimation, &QObject::deleteLater);
}

void BlackjackGUI::addCardToLayoutAnimated(QHBoxLayout* layout, const Card& card, int delayMs) {
    QWidget* parentWidget = (layout == dealerCardLayout) ? dealerCardWidget : playerCardWidget;
    QLabel* cardLabel = cardImageManager->createCardLabel(card, parentWidget);
    
    // Remove stretch before adding new card
    QLayoutItem* stretch = layout->takeAt(layout->count() - 1);
    if (stretch && stretch->spacerItem()) {
        delete stretch;
    }
    
    layout->addWidget(cardLabel);
    
    // Add stretch back
    layout->addStretch();
    
    // Force immediate layout update
    parentWidget->updateGeometry();
    
    // Simple fade-in animation instead of complex movements
    QGraphicsOpacityEffect* opacity = new QGraphicsOpacityEffect(cardLabel);
    cardLabel->setGraphicsEffect(opacity);
    
    QPropertyAnimation* fadeAnimation = new QPropertyAnimation(opacity, "opacity");
    fadeAnimation->setDuration(500);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->setEasingCurve(QEasingCurve::OutQuart);
    
    // Add delay if specified
    if (delayMs > 0) {
        QTimer::singleShot(delayMs, [fadeAnimation]() {
            fadeAnimation->start();
        });
    } else {
        fadeAnimation->start();
    }
    
    // Clean up animation when finished
    connect(fadeAnimation, &QAbstractAnimation::finished, fadeAnimation, &QObject::deleteLater);
}

// Animated back card method
void BlackjackGUI::addBackCardToLayoutAnimated(QHBoxLayout* layout, int delayMs) {
    QLabel* backLabel = cardImageManager->createBackCardLabel();
    layout->addWidget(backLabel);
    
    // Start off-screen and animate in
    QPoint finalPosition = backLabel->pos();
    backLabel->move(backLabel->x() + 200, backLabel->y());
    
    QPropertyAnimation* slideAnimation = new QPropertyAnimation(backLabel, "pos");
    slideAnimation->setDuration(500);
    slideAnimation->setStartValue(QPoint(backLabel->x(), backLabel->y()));
    slideAnimation->setEndValue(finalPosition);
    slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    if (delayMs > 0) {
        QTimer::singleShot(delayMs, [slideAnimation]() {
            slideAnimation->start();
        });
    } else {
        slideAnimation->start();
    }
    
    connect(slideAnimation, &QAbstractAnimation::finished, slideAnimation, &QObject::deleteLater);
}



void BlackjackGUI::logMessage(const QString& message) {
    gameLog->append(message);
    gameLog->ensureCursorVisible();
}

// Helper methods you'll need to implement
QString BlackjackGUI::formatDealerHand(bool showHoleCard) const {
    const Dealer& dealer = gameEngine->getDealer();
    if (dealer.getCardCount() == 0) {
        return "Dealer: No cards";
    }
    
    QString result = "Dealer: ";
    if (showHoleCard || dealer.isHoleCardRevealed()) {
        result += QString("Total: %1").arg(dealer.getTotalValue());
    } else {
        result += QString("Showing: %1").arg(dealer.getCard(0).getValue() > 10 ? 10 : dealer.getCard(0).getValue());
    }
    
    return result;
}

QString BlackjackGUI::formatPlayerHand(int playerIndex) const {
    const std::vector<Player>& players = gameEngine->getPlayers();
    if (playerIndex >= 0 && playerIndex < static_cast<int>(players.size())) {
        const Player& player = players[playerIndex];
        return QString("Player: Total: %1").arg(player.getTotalValue());
    }
    return "Player: No cards";
}

void BlackjackGUI::flipHoleCard(QLabel* cardLabel, const Card& revealedCard) {
    if (!cardLabel) return;
    
    // Get the original geometry
    QRect originalGeometry = cardLabel->geometry();
    
    // Create a 3D-like flip effect by scaling width
    QPropertyAnimation* flipOut = new QPropertyAnimation(cardLabel, "geometry");
    QRect slimGeometry = originalGeometry;
    slimGeometry.setX(originalGeometry.x() + originalGeometry.width() / 2 - 2);
    slimGeometry.setWidth(4); // Very thin
    
    flipOut->setDuration(250);
    flipOut->setStartValue(originalGeometry);
    flipOut->setEndValue(slimGeometry);
    flipOut->setEasingCurve(QEasingCurve::InOutQuart);
    
    connect(flipOut, &QPropertyAnimation::finished, [this, cardLabel, revealedCard, originalGeometry]() {
        // Change the card image at the halfway point
        QPixmap cardImage = cardImageManager->getCardImage(revealedCard);
        cardLabel->setPixmap(cardImage.scaled(cardImageManager->getCardWidth(), 
                                            cardImageManager->getCardHeight(), 
                                            Qt::KeepAspectRatio, 
                                            Qt::SmoothTransformation));
        
        // Flip back to full width
        QPropertyAnimation* flipIn = new QPropertyAnimation(cardLabel, "geometry");
        flipIn->setDuration(250);
        flipIn->setStartValue(cardLabel->geometry());
        flipIn->setEndValue(originalGeometry);
        flipIn->setEasingCurve(QEasingCurve::InOutQuart);
        flipIn->start();
        
        connect(flipIn, &QAbstractAnimation::finished, flipIn, &QObject::deleteLater);
    });
    
    flipOut->start();
    connect(flipOut, &QAbstractAnimation::finished, flipOut, &QObject::deleteLater);
}


void BlackjackGUI::onClearBet() {

    if (currentBet > 0) {
        playerBalance += currentBet;
    }
    currentBet = 0.0;
    betAmountSpinBox->setValue(static_cast<int>(minimumBet)); 
    updateBettingDisplay();
    logMessage("🚫 Bet cleared");
    newGameButton->setEnabled(false);
    placeBetButton->setEnabled(true);
}

void BlackjackGUI::onBetAmountChanged(double amount) { 
    (void)amount;
    
    // Update max based on current balance
    double maxAffordable = std::min(maximumBet, playerBalance);
    betAmountSpinBox->setMaximum(maxAffordable);
    
    // Clear current bet when amount changes
    if (currentBet > 0) {
        currentBet = 0.0;
        updateBettingDisplay();
        newGameButton->setEnabled(false);
        placeBetButton->setEnabled(true);
        updateGameButtonStates();
    }
}

// Update betting display
void BlackjackGUI::updateBettingDisplay() {
    balanceLabel->setText(QString("💳 Balance: $%1").arg(playerBalance, 0, 'f', 2));
    betLabel->setText(QString("🎯 Current Bet: $%1").arg(currentBet, 0, 'f', 2));
    
    // Update spinbox maximum based on current balance
    double maxAffordable = std::min(maximumBet, playerBalance);
    betAmountSpinBox->setMaximum(maxAffordable);
    
    // Update button states
    placeBetButton->setEnabled(currentBet == 0.0 && playerBalance >= minimumBet);
    maxBetButton->setEnabled(currentBet == 0.0 && playerBalance >= minimumBet);
    clearBetButton->setEnabled(currentBet > 0.0);
}


// Process bet results
void BlackjackGUI::processBetResult(GameResult result, bool isBlackjack, bool doubledDown) {

    double baseBet = currentBet;
    
    double payout = 0.0;
    QString resultMessage;
    
    switch (result) {
        case GameResult::WIN:
            if (doubledDown) {
                // Win: return both bets + winnings for both
                payout = baseBet * 4; // (original bet + double bet) + winnings for both
                resultMessage = QString("🎉 DOUBLED WIN! +$%1").arg(baseBet * 3, 0, 'f', 2); // Show net gain
            } else {
                // Regular win: return bet + equal winnings
                payout = baseBet * 2; // Return bet + winnings
                resultMessage = QString("🎉 YOU WIN! +$%1").arg(baseBet, 0, 'f', 2); // Show net gain
            }
            break;
            
        case GameResult::BLACKJACK:
            // Blackjack: return bet + 1.5x winnings
            payout = baseBet + (baseBet * 1.5);
            resultMessage = QString("🎊 BLACKJACK! +$%1").arg(baseBet * 1.5, 0, 'f', 2); // Show net gain
            break;
            
        case GameResult::PUSH:
            if (doubledDown) {
                // Push: return both bets
                payout = baseBet * 2;
                resultMessage = "🤝 PUSH - Doubled bet returned";
            } else {
                // Regular push: return original bet
                payout = baseBet;
                resultMessage = "🤝 PUSH - Bet returned";
            }
            break;
            
        case GameResult::LOSS:
            // Loss: nothing returned (money already gone)
            payout = 0.0;
            if (doubledDown) {
                resultMessage = QString("💸 DOUBLED LOSS! Total lost: $%1").arg(baseBet * 2, 0, 'f', 2);
            } else {
                resultMessage = QString("💸 YOU LOSE! Lost: $%1").arg(baseBet, 0, 'f', 2);
            }
            break;
    }
    
    playerBalance += payout;
    updateBettingDisplay();
    logMessage(resultMessage);

    if(gameEngine && gameEngine->getGameStats()){
        gameEngine->getGameStats()->updatePlayerStats("Player", result);
        updateGameStats();
    }
    
    // Log balance change for debugging
    logMessage(QString("💳 New balance: $%1").arg(playerBalance, 0, 'f', 2));
}

void BlackjackGUI::resetForNewBet() {
    
    endCurrentGame();

    currentBet = 0.0;
    betAmountSpinBox->setValue(minimumBet);
    updateBettingDisplay();
    
    // Reset insurance state
    insuranceOffered = false;
    insuranceAmount = 0.0;
    insuranceButton->setEnabled(false);
    insuranceButton->setText("🛡️ Insurance");
    
    // Re-enable betting controls
    placeBetButton->setEnabled(playerBalance >= minimumBet);
    maxBetButton->setEnabled(playerBalance >= minimumBet);
    newGameButton->setEnabled(false); // Will be enabled when bet is placed
    updateGameButtonStates();
    
    // Check if player can still play
    if (playerBalance < minimumBet) {
        QMessageBox::information(this, "Game Over", 
            "You don't have enough money to continue playing!\nStarting balance reset to $1000.");
        playerBalance = 1000.0;
        updateBettingDisplay();
    }
}

void BlackjackGUI::enableActions(bool enabled) {
    hitButton->setEnabled(enabled);
    standButton->setEnabled(enabled);
    
    bool canDoubleDown = enabled;
    if (enabled && gameEngine) {
        const std::vector<Player>& players = gameEngine->getPlayers();
        if (!players.empty()) {
            // Disable double down if player has more than 2 cards
            if (players[0].getCardCount() > 2) {
                canDoubleDown = false;
            }
            // Also check if player has enough money
            if (canDoubleDown && playerBalance < currentBet) {
                canDoubleDown = false;
                doubleButton->setText("💰 Double (Need $" + QString::number(currentBet, 'f', 2) + ")");
            } else if (canDoubleDown) {
                doubleButton->setText("💰 Double Down");
            }
        }
    }
    
    doubleButton->setEnabled(canDoubleDown);
    surrenderButton->setEnabled(enabled);
    if (enabled) {
        updateSplitButtonState();
        
        // Also check if player has enough money for split
        if (splitButton->isEnabled() && playerBalance < currentBet) {
            splitButton->setEnabled(false);
            splitButton->setText("✂️ Split (Need $" + QString::number(currentBet, 'f', 2) + ")");
        }
        
        // Check if player has enough money for double
        if (playerBalance < currentBet) {
            doubleButton->setEnabled(false);
            doubleButton->setText("💰 Double (Need $" + QString::number(currentBet, 'f', 2) + ")");
        } else if (enabled) {
            doubleButton->setText("💰 Double Down");
        }
        
        //Surrender only available on first 2 cards
        const std::vector<Player>& players = gameEngine->getPlayers();
        if (!players.empty() && players[0].getCardCount() > 2) {
            surrenderButton->setEnabled(false);
        }
        
    } else {
        splitButton->setEnabled(false);
        splitButton->setText("✂️ Split");
        surrenderButton->setEnabled(false);  // NEW
    }
    
    // Insurance button - only enable if offered and enabled
    if (!enabled || !insuranceOffered) {
        insuranceButton->setEnabled(false);
        if (!insuranceOffered) {
            insuranceButton->setText("🛡️ Insurance");
        }
    }
    
    // Update game button states
    updateGameButtonStates();
}
void BlackjackGUI::updateSplitButtonState() {
    if (gameEngine && gameEngine->canPlayerSplit(0)) {  // Check if player 0 can split
        splitButton->setEnabled(true);
        splitButton->setStyleSheet(
            "background-color: #2ecc71; color: white; font-weight: bold; "
            "border: 2px solid #27ae60; min-width: 120px;"
        );
        splitButton->setText("✂️ Split (Available!)");
    } else {
        splitButton->setEnabled(false);
        splitButton->setStyleSheet(
            "background-color: #7f8c8d; color: #bdc3c7; "
            "border: 2px solid #95a5a6; min-width: 120px;"
        );
        splitButton->setText("✂️ Split (Not Available)");
    }
}

void BlackjackGUI::onSplitClicked() {
    if (!gameEngine || !gameEngine->canPlayerSplit(0)) {
        return;
    }
    
    logMessage("Player chooses to SPLIT");
    
    // Check if player has enough money for additional bet
    if (playerBalance < currentBet) {
        QMessageBox::warning(this, "Insufficient Funds", 
            QString("You need $%1 more to split!\nYour balance: $%2")
            .arg(currentBet, 0, 'f', 2)
            .arg(playerBalance, 0, 'f', 2));
        return;
    }
    
    // *** DEDUCT ADDITIONAL BET FOR SPLIT ***
    playerBalance -= currentBet; 
    updateBettingDisplay();
    logMessage(QString("💸 Additional bet for split: -$%1").arg(currentBet, 0, 'f', 2));
    logMessage(QString("💰 Total at risk: $%1 (on 2 hands)").arg(currentBet * 2, 0, 'f', 2));
    
    // Perform the split
    bool success = gameEngine->playerSplits(0);
    
    if (success) {
        // DISABLE ALL ACTIONS TO PREVENT INFINITE LOOP
        enableActions(false);
        
        // Set up split hand tracking
        isPlayingSplitHands = true;
        currentSplitHandIndex = 0;
        
        // Get the number of split hands from the split manager
        auto splitManager = gameEngine->getSplitManager();
        if (splitManager) {
            auto& splitHands = splitManager->getPlayerSplitHands();
            if (splitHands.find(0) != splitHands.end()) {
                totalSplitHands = static_cast<int>(splitHands.at(0).size());
            } else {
                totalSplitHands = 0;
            }
        }
        
        logMessage(QString("🔄 Split successful! Playing hand 1 of %1").arg(totalSplitHands));
        statusLabel->setText(QString("🎯 Status: Playing split hand 1 of %1 - choose action!").arg(totalSplitHands));
        
        // Force update display ONCE
        updateDisplay();
        
        // Re-enable actions for split hand play AFTER display update
        QTimer::singleShot(100, [this]() {
            enableActions(true);
            splitButton->setEnabled(false); // Disable split during split play
        });
        
    } else {
        logMessage("❌ Split failed!");
        // Return the extra bet if split failed
        playerBalance += currentBet;
        updateBettingDisplay();
    }
}

void BlackjackGUI::handleSplitHandCompletion() {
    currentSplitHandIndex++;
    
    if (currentSplitHandIndex < totalSplitHands) {
        // Move to next split hand
        logMessage(QString("🔄 Moving to split hand %1 of %2").arg(currentSplitHandIndex + 1).arg(totalSplitHands));
        statusLabel->setText(QString("🎯 Status: Playing split hand %1 of %2 - choose action!").arg(currentSplitHandIndex + 1).arg(totalSplitHands));
        
        // Player continues with next hand
        enableActions(true);
        updateDisplay();
    } else {
        // All split hands completed - dealer turn
        logMessage("🏁 All split hands completed - Dealer's turn");
        statusLabel->setText("🎯 Status: All split hands complete - Dealer's turn...");
        
        isPlayingSplitHands = false;
        enableActions(false);
        
        // Play dealer turn
        gameEngine->playDealerTurn();
        updateDisplay();
        
        // Process results for all split hands
        processSplitHandResults();
        resetForNewBet();
    }
}

// New method to process results for regular games:
void BlackjackGUI::processRegularGameResult() {
    const std::vector<Player>& players = gameEngine->getPlayers();
    const Dealer& dealer = gameEngine->getDealer();

    if(!dealer.isHoleCardRevealed()){
        const_cast<Dealer&>(dealer).revealHoleCard();
        updateDisplay();
    }
    
    if (!players.empty()) {
        const Player& player = players[0];
        
        if (dealer.isBusted()) {
            logMessage("🎉 Dealer busts! You win!");
            statusLabel->setText("🎯 Status: You win! Dealer busted!");
            processBetResult(GameResult::WIN);
        } else if (player.getTotalValue() > dealer.getTotalValue()) {
            logMessage("🎉 You win with higher total!");
            statusLabel->setText("🎯 Status: You win!");
            processBetResult(GameResult::WIN);
        } else if (player.getTotalValue() < dealer.getTotalValue()) {
            logMessage("💸 You lose - dealer has higher total");
            statusLabel->setText("🎯 Status: You lose!");
            processBetResult(GameResult::LOSS);
        } else {
            logMessage("🤝 Push! Same total as dealer");
            statusLabel->setText("🎯 Status: Push!");
            processBetResult(GameResult::PUSH);
        }

        updateGameStats();
    }
}

// New method to process split hand results:
void BlackjackGUI::processSplitHandResults() {
    auto splitManager = gameEngine->getSplitManager();
    if (!splitManager) {
        logMessage("❌ Error: No split manager found");
        return;
    }
    
    auto& splitHands = splitManager->getPlayerSplitHands();
    if (splitHands.find(0) == splitHands.end()) {
        logMessage("❌ Error: No split hands found");
        return;
    }
    
    const auto& playerSplitHands = splitHands.at(0);
    const Dealer& dealer = gameEngine->getDealer();
    
    int wins = 0, losses = 0, pushes = 0;
    double totalPayout = 0.0;
    
    for (size_t i = 0; i < playerSplitHands.size(); ++i) {
        const auto& hand = playerSplitHands[i];
        double handBet = currentBet;
        
        // Check if this hand was doubled (you might need to add this info to your SplitHands struct)
        // For now, assuming we can check bet multiplier
        if (hand.getBetMultiplier() > 1) {
            handBet *= hand.getBetMultiplier();
        }
        
        GameResult result;
        if (hand.isBusted()) {
            result = GameResult::LOSS;
            losses++;
            logMessage(QString("💸 Split hand %1: BUST - Lost $%2").arg(i + 1).arg(handBet, 0, 'f', 2));
        } else if (dealer.isBusted()) {
            result = GameResult::WIN;
            wins++;
            totalPayout += handBet * 2;
            logMessage(QString("🎉 Split hand %1: WIN (dealer bust) - Won $%2").arg(i + 1).arg(handBet, 0, 'f', 2));
        } else if (hand.getTotalValue() > dealer.getTotalValue()) {
            result = GameResult::WIN;
            wins++;
            totalPayout += handBet * 2;
            logMessage(QString("🎉 Split hand %1: WIN (%2 vs %3) - Won $%4").arg(i + 1).arg(hand.getTotalValue()).arg(dealer.getTotalValue()).arg(handBet, 0, 'f', 2));
        } else if (hand.getTotalValue() < dealer.getTotalValue()) {
            result = GameResult::LOSS;
            losses++;
            logMessage(QString("💸 Split hand %1: LOSS (%2 vs %3) - Lost $%4").arg(i + 1).arg(hand.getTotalValue()).arg(dealer.getTotalValue()).arg(handBet, 0, 'f', 2));
        } else {
            result = GameResult::PUSH;
            pushes++;
            totalPayout += handBet;
            logMessage(QString("🤝 Split hand %1: PUSH (%2 vs %3) - Bet returned").arg(i + 1).arg(hand.getTotalValue()).arg(dealer.getTotalValue()));
        }
    }
    
    // Update balance with total payout
    playerBalance += totalPayout;
    updateBettingDisplay();
    
    // Summary message
    logMessage(QString("📊 Split Results: %1 wins, %2 losses, %3 pushes").arg(wins).arg(losses).arg(pushes));
    
    if (wins > losses) {
        statusLabel->setText("🎯 Status: Split complete - Overall WIN!");
    } else if (losses > wins) {
        statusLabel->setText("🎯 Status: Split complete - Overall LOSS!");
    } else {
        statusLabel->setText("🎯 Status: Split complete - Break even!");
    }
}

//Insurnace method
void BlackjackGUI::offerInsurance() {
    insuranceOffered = true;
    insuranceAmount = currentBet / 2.0; // Insurance is half the original bet
    
    // Create insurance dialog
    QMessageBox insuranceDialog;
    insuranceDialog.setWindowTitle("Insurance Offer");
    insuranceDialog.setText("🛡️ INSURANCE OFFER 🛡️");
    insuranceDialog.setInformativeText(
        QString("The dealer is showing an Ace!\n\n"
                "Purchase insurance for $%1?\n"
                "• Insurance costs half your original bet\n"
                "• Pays 2:1 if dealer has blackjack\n"
                "• You lose insurance if dealer doesn't have blackjack\n\n"
                "Original bet: $%2\nInsurance cost: $%3")
        .arg(insuranceAmount, 0, 'f', 2)
        .arg(currentBet, 0, 'f', 2)
        .arg(insuranceAmount, 0, 'f', 2));
    
    insuranceDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    insuranceDialog.setDefaultButton(QMessageBox::No);
    insuranceDialog.button(QMessageBox::Yes)->setText("Buy Insurance ($" + QString::number(insuranceAmount, 'f', 2) + ")");
    insuranceDialog.button(QMessageBox::No)->setText("No Insurance");
    
    // Check if player has enough money
    if (playerBalance < insuranceAmount) {
        insuranceDialog.setStandardButtons(QMessageBox::Ok);
        insuranceDialog.setText("❌ INSUFFICIENT FUNDS");
        insuranceDialog.setInformativeText(
            QString("You need $%1 for insurance but only have $%2 available.")
            .arg(insuranceAmount, 0, 'f', 2)
            .arg(playerBalance, 0, 'f', 2));
        insuranceDialog.button(QMessageBox::Ok)->setText("Continue Without Insurance");
        insuranceDialog.exec();
        
        // Continue game without insurance
        insuranceOffered = false;
        enableActions(true);
        placeBetButton->setEnabled(false);
        updateSplitButtonState();
        return;
    }
    
    int choice = insuranceDialog.exec();
    
    if (choice == QMessageBox::Yes) {
        // Player buys insurance
        playerBalance -= insuranceAmount;
        updateBettingDisplay();
        
        logMessage(QString("💸 Insurance purchased: -$%1").arg(insuranceAmount, 0, 'f', 2));
        
        // Check if dealer has blackjack
        const Dealer& dealer = gameEngine->getDealer();
        if (dealer.checkForBlackjack()) {
            logMessage("✅ Insurance pays! Dealer has blackjack!");
            logMessage(QString("💰 Insurance payout: +$%1").arg(insuranceAmount * 2, 0, 'f', 2));
            
            // Pay insurance (2:1)
            playerBalance += insuranceAmount * 3; // Return bet + 2:1 payout
            updateBettingDisplay();
            
            // Reveal dealer cards and end game
            gameEngine->playDealerTurn();
            updateDisplay();
            
            // Process main bet result
            const std::vector<Player>& players = gameEngine->getPlayers();
            if (!players.empty() && players[0].isBlackjack()) {
                logMessage("🤝 Main bet pushes (both have blackjack)");
                statusLabel->setText("🎯 Status: Insurance won, main bet pushes!");
                processBetResult(GameResult::PUSH, true);
            } else {
                logMessage("💸 Main bet loses to dealer blackjack");
                statusLabel->setText("🎯 Status: Insurance won, main bet loses!");
                processBetResult(GameResult::LOSS);
            }
            
            enableActions(false);
            resetForNewBet();
        } else {
            logMessage("❌ Insurance loses - dealer doesn't have blackjack");
            statusLabel->setText("🎯 Status: Insurance lost - continue playing!");
            
            // Continue with normal game
            enableActions(true);
            placeBetButton->setEnabled(false);
            updateSplitButtonState();
        }
    } else {
        // Player declines insurance
        logMessage("🚫 Insurance declined");
        
        // Continue with normal game
        enableActions(true);
        placeBetButton->setEnabled(false);
        updateSplitButtonState();
    }
    
    insuranceOffered = false;
}

void BlackjackGUI::endCurrentGame() {
 
    if (gameEngine) {
        gameEngine->endGame(); // This sets state to GAME_OVER
    }
    
    // Update button states immediately
    updateGameButtonStates();
    
    logMessage("🏁 Game ended - ready for new bet");
}
