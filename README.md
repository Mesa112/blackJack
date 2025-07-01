# Blackjack Simulator

A blackjack game built with C++ and Qt, featuring card counting training and basic strategy analysis.

## Features

- Complete blackjack implementation with standard casino rules
- Hi-Lo card counting system with accuracy tracking
- Basic strategy trainer with performance statistics
- Qt GUI with card animations and betting interface
- Support for splits, doubles, insurance, and surrender
- 8-deck shoe simulation with proper shuffling

## Screenshots

![Game Interface](screenshots/main-game.png)
*Main game interface*

![Card Counting](screenshots/counting.png)
*Card counting training mode*

## Building

### Requirements
- Qt 6.0 or higher
- C++17 compatible compiler

### Build Instructions
```bash
git clone https://github.com/yourusername/blackjack-simulator.git
cd blackjack-simulator
qmake
make
./Blackjack
```

## Game Rules

Standard blackjack rules apply:
- Dealer hits on soft 17
- Blackjack pays 3:2
- Double down on any two cards
- Split pairs up to 4 hands
- Insurance available when dealer shows Ace

## Card Counting

The Hi-Lo counting system is implemented:
- Low cards (2-6): +1
- Neutral cards (7-9): 0  
- High cards (10-A): -1

The game tracks running count, true count, and provides betting recommendations based on the current count.

## Architecture

```
BlackjackGame/
├── Card.h/cpp           # Card representation
├── Deck.h/cpp           # 8-deck shoe management
├── Hand.h/cpp           # Hand evaluation logic
├── Player.h/cpp         # Player actions and state
├── Dealer.h/cpp         # Dealer behavior
├── GameEngine.h/cpp     # Game flow control
├── Stats.h/cpp          # Statistics tracking
├── Counting.h/cpp       # Card counting systems
├── BasicStrategy.h/cpp  # Strategy analysis
├── SplitHand.h/cpp      # Split hand management
└── BlackjackGUI.h/cpp   # Qt interface
```

## Technical Details

- Implements proper blackjack mathematics and probabilities
- Uses Qt's animation framework for smooth card dealing
- Maintains game statistics and strategy accuracy tracking
- Handles complex game states like split hands and side bets

## License

MIT License - see LICENSE file for details.