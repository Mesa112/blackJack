# 🃏 Blackjack Simulator

<div align="center">

![Blackjack Logo](screenshots/logo.png)

**A professional casino-style blackjack game with advanced card counting and strategy training**

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-6.0+-41CD52?style=for-the-badge&logo=qt)](https://www.qt.io/)
[![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)](LICENSE)
[![Stars](https://img.shields.io/github/stars/yourusername/blackjack-simulator?style=for-the-badge)](https://github.com/yourusername/blackjack-simulator/stargazers)

[**🎮 Download Latest Release**](https://github.com/yourusername/blackjack-simulator/releases/latest) • [**📖 Documentation**](docs/) • [**🐛 Report Bug**](https://github.com/yourusername/blackjack-simulator/issues) • [**💡 Request Feature**](https://github.com/yourusername/blackjack-simulator/issues)

</div>

---

## 🎯 What Makes This Special?

<table>
<tr>
<td width="50%">

### 🧠 **Smart Training System**
- **Hi-Lo Card Counting** with real-time accuracy tracking
- **Basic Strategy Trainer** that corrects your mistakes
- **Performance Analytics** to monitor your improvement
- **Quiz System** for testing your counting skills

</td>
<td width="50%">

### 🎲 **Authentic Casino Experience**
- **8-Deck Shoe** with professional shuffling
- **Complete Rule Set** - splits, doubles, insurance, surrender
- **Realistic Betting** with bankroll management
- **Smooth Animations** for immersive gameplay

</td>
</tr>
</table>

---

## 🚀 Quick Start

<details>
<summary><b>🔧 Installation (Click to expand)</b></summary>

### Prerequisites
```bash
# Install Qt (Ubuntu/Debian)
sudo apt-get install qt6-base-dev qt6-tools-dev

# Install Qt (macOS)
brew install qt@6

# Install Qt (Windows)
# Download from https://www.qt.io/download
```

### Build & Run
```bash
git clone https://github.com/yourusername/blackjack-simulator.git
cd blackjack-simulator
qmake Blackjack.pro
make
./Blackjack
```

</details>

---

## 📸 See It In Action

<div align="center">

### 🎮 Main Game Interface
![Main Game](screenshots/main-game.png)
*Professional dealer interface with real-time statistics*

</div>

<details>
<summary><b>📷 More Screenshots (Click to view)</b></summary>

| Card Counting Training | Split Hands Management | Statistics Dashboard |
|------------------------|------------------------|---------------------|
| ![Counting](screenshots/counting.png) | ![Split](screenshots/split.png) | ![Stats](screenshots/stats.png) |
| *Hi-Lo system with accuracy tracking* | *Advanced split hand gameplay* | *Comprehensive performance analytics* |

</details>

---

## 🎯 Core Features

<table>
<tr>
<td align="center" width="33%">
<img src="https://img.icons8.com/color/96/000000/playing-cards.png" width="80"/>

### 🃏 **Complete Blackjack**
Full casino rules with splits, doubles, insurance, surrender, and even money
</td>
<td align="center" width="33%">
<img src="https://img.icons8.com/color/96/000000/brain.png" width="80"/>

### 🧠 **Card Counting**
Hi-Lo system with running/true count tracking and betting recommendations
</td>
<td align="center" width="33%">
<img src="https://img.icons8.com/color/96/000000/statistics.png" width="80"/>

### 📊 **Analytics**
Detailed performance tracking with strategy accuracy monitoring
</td>
</tr>
</table>

---

## 🎓 Educational Value

> **Perfect for learning optimal blackjack strategy and card counting techniques**

<details>
<summary><b>📚 What You'll Learn</b></summary>

- **Card Counting Fundamentals**: Master the Hi-Lo system with guided practice
- **Basic Strategy Mastery**: Learn mathematically optimal decisions for every situation
- **Bankroll Management**: Understand proper betting strategies and risk control
- **Game Mathematics**: Explore the statistical foundations behind blackjack
- **Real-time Decision Making**: Practice under realistic casino conditions

</details>

---

## 🏗️ Technical Architecture

<details>
<summary><b>⚙️ System Design (Click to expand)</b></summary>

```
🎮 BlackjackSimulator/
├── 🃏 Core Game Logic
│   ├── Card.h/cpp              # Card representation
│   ├── Deck.h/cpp              # 8-deck shoe management
│   ├── Hand.h/cpp              # Hand evaluation
│   ├── Player.h/cpp            # Player actions
│   └── Dealer.h/cpp            # Dealer behavior
├── 🎯 Game Engine
│   ├── GameEngine.h/cpp        # Main game controller
│   ├── SplitHand.h/cpp         # Split hand logic
│   └── Stats.h/cpp             # Statistics tracking
├── 🧠 Training Systems
│   ├── Counting.h/cpp          # Card counting algorithms
│   └── BasicStrategy.h/cpp     # Strategy analysis
└── 🎨 User Interface
    ├── BlackjackGUI.h/cpp      # Qt interface
    └── CardImageManager.h/cpp  # Visual components
```

**Tech Stack:**
- **Language**: C++17
- **GUI Framework**: Qt 6.x
- **Build System**: qmake/CMake
- **Platforms**: Windows, macOS, Linux

</details>

---

## 🎮 Game Rules & Controls

<details>
<summary><b>🕹️ How to Play</b></summary>

### Basic Controls
| Action | Description |
|--------|-------------|
| **Hit** | Take another card |
| **Stand** | Keep current total |
| **Double Down** | Double bet, take one card |
| **Split** | Split matching pairs |
| **Surrender** | Forfeit half your bet |
| **Insurance** | Side bet when dealer shows Ace |

### Casino Rules
- Dealer hits on soft 17
- Blackjack pays 3:2
- Double down on any two cards
- Split pairs up to 4 hands
- Insurance pays 2:1
- Surrender returns half bet

</details>

---

## 🎯 Card Counting System

<div align="center">

### Hi-Lo Counting Values

| Cards | Value | Effect |
|-------|-------|--------|
| **2, 3, 4, 5, 6** | +1 | 🔴 Favor Player |
| **7, 8, 9** | 0 | ⚪ Neutral |
| **10, J, Q, K, A** | -1 | 🔵 Favor Dealer |

</div>

<details>
<summary><b>🧮 Counting Features</b></summary>

- **Running Count**: Real-time count maintenance
- **True Count**: Adjusted for remaining decks  
- **Betting Advice**: Recommended bet sizing
- **Accuracy Tracking**: Monitor counting precision
- **Quiz Mode**: Test your skills with random challenges

</details>

---

## 📊 Performance Tracking

<div align="center">

![Performance Chart](screenshots/performance-chart.png)
*Track your improvement over time*

</div>

The simulator provides comprehensive analytics:

- 📈 **Win Rate Trends** over multiple sessions
- 🎯 **Strategy Accuracy** percentage with mistake analysis  
- 🧠 **Counting Precision** with detailed feedback
- 💰 **Bankroll Performance** and risk assessment
- 📋 **Session Summaries** with actionable insights

---

## 🛠️ Development

<details>
<summary><b>👥 Contributing</b></summary>

We welcome contributions! Here's how:

1. 🍴 **Fork** the repository
2. 🌿 **Create** your feature branch (`git checkout -b feature/amazing-feature`)
3. 💾 **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. 📤 **Push** to the branch (`git push origin feature/amazing-feature`)
5. 🔄 **Open** a Pull Request

### Development Setup
```bash
# Install development tools
sudo apt-get install qt6-base-dev qt6-tools-dev cmake

# Build in debug mode
qmake CONFIG+=debug
make

# Run tests (if available)
make test
```

</details>

---

## 🗺️ Roadmap

- [ ] 🎯 **Additional Counting Systems** (KO, Hi-Opt I, Omega II)
- [ ] 🌐 **Web Version** with WebAssembly
- [ ] 📱 **Mobile Apps** for iOS/Android
- [ ] 🏆 **Tournament Mode** with leaderboards
- [ ] 🤖 **AI Opponents** with different skill levels
- [ ] 🔊 **Sound Effects** and music
- [ ] 📊 **Advanced Analytics** with ML insights

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<div align="center">

### 🎲 Ready to Master Blackjack?

[![Download](https://img.shields.io/badge/Download-Latest%20Release-success?style=for-the-badge&logo=download)](https://github.com/yourusername/blackjack-simulator/releases/latest)
[![Star](https://img.shields.io/badge/⭐-Star%20This%20Repo-yellow?style=for-the-badge)](https://github.com/yourusername/blackjack-simulator)

**Built with ❤️ by passionate developers**

[🐛 Report Issues](https://github.com/yourusername/blackjack-simulator/issues) • [💬 Discussions](https://github.com/yourusername/blackjack-simulator/discussions) • [📧 Contact](mailto:your.email@example.com)

</div>
