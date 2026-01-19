# Installation Guide

## System Requirements

- **OS**: Linux (Ubuntu 18.04+, Debian, Fedora), macOS 10.12+, or Windows 10+
- **RAM**: Minimum 512MB
- **Disk Space**: ~50MB for game + SFML libraries
- **Display**: 800x600 minimum resolution

## Installing SFML 2.5.1

### Linux

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libsfml-dev
```

#### Fedora
```bash
sudo dnf install SFML-devel
```

#### Arch Linux
```bash
sudo pacman -S sfml
```

### macOS

Using Homebrew:
```bash
brew install sfml
```

### Windows

1. Download SFML 2.5.1 from: https://www.sfml-dev.org/download/sfml/2.5.1/
2. Choose the version matching your compiler (Visual Studio, MinGW, etc.)
3. Extract to `C:\SFML-2.5.1\`
4. Update the build script paths accordingly

## Building the Project

### Linux/macOS

1. **Clone the repository**
   ```bash
   git clone https://github.com/YOUR_USERNAME/xonix-game.git
   cd xonix-game
   ```

2. **Make the build script executable**
   ```bash
   chmod +x p.sh
   ```

3. **Build and run**
   ```bash
   ./p.sh
   ```

### Windows

1. **Using MinGW:**
   ```cmd
   g++ -c main.cpp Auth.cpp UserDB.cpp Level.cpp Player.cpp Multiplayer.cpp ^
       Leaderboard.cpp SaveGame.cpp Friends.cpp ScoreSystem.cpp MatchMaking.cpp ^
       AVLTree.cpp Inventory.cpp -IC:\SFML-2.5.1\include

   g++ main.o Auth.o UserDB.o Player.o Leaderboard.o Level.o ScoreSystem.o ^
       Friends.o Multiplayer.o SaveGame.o MatchMaking.o AVLTree.o Inventory.o ^
       -o game.exe -LC:\SFML-2.5.1\lib ^
       -lsfml-graphics -lsfml-window -lsfml-system

   game.exe
   ```

2. **Using CMake:**
   ```cmd
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

## Troubleshooting

### Error: "cannot open shared object file"

**Linux:**
```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

Or if using local SFML:
```bash
export LD_LIBRARY_PATH="$PWD/SFML-2.5.1/lib:$LD_LIBRARY_PATH"
```

**macOS:**
```bash
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
```

### Error: "SFML/Graphics.hpp: No such file or directory"

Ensure SFML is properly installed and the include path is correct in your build command.

**Linux (system install):**
```bash
sudo ldconfig
pkg-config --cflags --libs sfml-all
```

### Fonts/Images Not Loading

Ensure you run the game from the project root directory where `fonts/` and `images/` folders exist.

```bash
cd /path/to/xonix-game
./game
```

## First Run

On first run, you'll need to:

1. **Register a new account**
   - Choose "Register" from main menu
   - Enter username and password
   - Confirm password

2. **Start playing**
   - Login with your credentials
   - Select "Start Game"
   - Choose your level

## Development Setup

For development, install additional tools:

```bash
# GDB for debugging
sudo apt-get install gdb

# Valgrind for memory leak detection
sudo apt-get install valgrind

# Build with debug symbols
g++ -g -c main.cpp ...
```

## Verified Platforms

✅ Ubuntu 20.04 LTS - gcc 9.4.0
✅ Debian 11 - gcc 10.2.1
✅ Arch Linux - gcc 13.2.1
✅ macOS Monterey - clang 13.0
⚠️ Windows 10 - MinGW-w64 (requires path adjustments)

---

For more help, open an issue on GitHub: https://github.com/YOUR_USERNAME/xonix-game/issues
