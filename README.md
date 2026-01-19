# Xonix - Tile Capture Arcade Game

A modern implementation of the classic Xonix arcade game built with C++ and SFML. Capture territory by drawing trails while avoiding enemies in this addictive puzzle-action game.


]
## 🎮 Features

- **Classic Xonix Gameplay**: Draw trails to capture territory while avoiding enemies
- **User Authentication**: Register and login system with persistent accounts
- **Multiplayer Mode**: Local 2-player co-op on the same keyboard
- **Matchmaking System**: Find and play with other players
- **Leaderboard**: Track high scores and compete globally
- **Save/Load System**: Save your progress and continue later
- **Friends System**: Add friends, send requests, and play together
- **Power-ups**: Collect and use special abilities
- **Multiple Levels**: Progress through different difficulty levels
- **Customizable Themes**: Choose from different visual themes

## 🏗️ Data Structures Used

This project demonstrates various data structures as part of a DSA course project:

- **2D Arrays**: Game grid for tile management and collision detection
- **Linked Lists**: Save game tile storage (TileNode)
- **AVL Trees**: Efficient user data organization
- **Struct Arrays**: Enemy and player state management
- **File-based Persistence**: Plain text files for user data, leaderboard, and inventory

## 📋 Prerequisites

Before building this project, you need:

- **C++ Compiler**: g++ (GCC 7.0+ recommended) or clang++
- **SFML 2.5.1**: Simple and Fast Multimedia Library
- **CMake** (optional): For building with CMake
- **Linux/Unix**: Tested on Linux (should work on Windows/Mac with modifications)

## 🚀 Installation

### 1. Install SFML

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libsfml-dev
```

**Fedora:**
```bash
sudo dnf install SFML-devel
```

**Arch Linux:**
```bash
sudo pacman -S sfml
```

**macOS (with Homebrew):**
```bash
brew install sfml
```

**Windows:**
Download SFML from [official website](https://www.sfml-dev.org/download.php)

### 2. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/xonix-game.git
cd xonix-game
```

### 3. Build the Project

**Option A: Using the build script (recommended)**
```bash
chmod +x p.sh
./p.sh
```

**Option B: Manual compilation**
```bash
# Compile
g++ -c main.cpp Auth.cpp UserDB.cpp Level.cpp Player.cpp Multiplayer.cpp \
    Leaderboard.cpp SaveGame.cpp Friends.cpp ScoreSystem.cpp MatchMaking.cpp \
    AVLTree.cpp Inventory.cpp -I./SFML-2.5.1/include

# Link
g++ main.o Auth.o UserDB.o Player.o Leaderboard.o Level.o ScoreSystem.o \
    Friends.o Multiplayer.o SaveGame.o MatchMaking.o AVLTree.o Inventory.o \
    -o game -L./SFML-2.5.1/lib -lsfml-graphics -lsfml-window -lsfml-system

# Run
export LD_LIBRARY_PATH="$PWD/SFML-2.5.1/lib:$LD_LIBRARY_PATH"
./game
```

**Option C: Using CMake**
```bash
mkdir build && cd build
cmake ..
make
./game
```

## 🎯 How to Play

### Controls

**Single Player:**
- Arrow Keys: Move player
- Space: Use power-up
- Esc: Pause menu

**Multiplayer (Player 2):**
- WASD: Move
- E: Use power-up

### Gameplay

1. **Start Game**: Register/Login from the main menu
2. **Draw Trails**: Move outside the safe zone to draw a trail
3. **Capture Territory**: Return to safe zone to capture the enclosed area
4. **Avoid Enemies**: Don't let enemies touch your trail
5. **Reach Target**: Capture 75% of the board to complete the level
6. **Use Power-ups**: Collect and activate special abilities

## 📁 Project Structure

```
.
├── main.cpp              # Main game loop
├── Auth.cpp/h            # Authentication system
├── UserDB.cpp/h          # User database management
├── Level.cpp/h           # Level management
├── Player.cpp/h          # Player mechanics
├── Multiplayer.cpp/h     # Multiplayer functionality
├── Leaderboard.cpp/h     # Score tracking
├── SaveGame.cpp/h        # Save/Load system
├── Friends.cpp/h         # Friends management
├── ScoreSystem.cpp/h     # Scoring logic
├── MatchMaking.cpp/h     # Matchmaking engine
├── AVLTree.cpp/h         # AVL tree implementation
├── Inventory.cpp/h       # Power-up inventory
├── Theme.h               # Theme definitions
├── TileNode.h            # Linked list for save data
├── fonts/                # Game fonts
├── images/               # Game graphics
├── saves/                # Save files
├── users.txt             # User accounts
├── leaderboard.txt       # High scores
├── friends.txt           # Friend relationships
├── powerups.txt          # Power-up definitions
└── themes.txt            # Theme configurations
```

## 📊 Features Overview

### Authentication System
- User registration and login
- Encrypted password storage
- Session management

### Leaderboard
- Global high scores
- Personal best tracking
- Persistent storage

### Save System
- Save game progress
- Load previous games
- Multiple save slots

### Friends System
- Send friend requests
- Accept/reject requests
- View friends list
- Play with friends

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the project
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 👥 Authors

- **Group 1** - *DSA Course Project*
  - Member 1: [Muhammad Wajid] - [Cyber-punk-69]
  - Member 2: [Muhammad Usman] - [Muhammad-Usman-Cy]

## 🙏 Acknowledgments

- SFML Library for graphics framework
- Original Xonix game for inspiration
- Course instructor and teaching assistants

## 📧 Contact

Project Link: [https://github.com/YOUR_USERNAME/xonix-game](https://github.com/Cyber-punk-69/xonix-game)

---

