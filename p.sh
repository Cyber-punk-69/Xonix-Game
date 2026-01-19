#!/bin/bash
# build_and_run.sh

# Set SFML path
export LD_LIBRARY_PATH="$PWD/SFML-2.5.1/lib:$LD_LIBRARY_PATH"

# Compile all source files
g++ -c main.cpp Auth.cpp UserDB.cpp Level.cpp Player.cpp  Multiplayer.cpp Leaderboard.cpp SaveGame.cpp Friends.cpp ScoreSystem.cpp MatchMaking.cpp AVLTree.cpp Inventory.cpp \
    -I./SFML-2.5.1/include

# Link everything
g++ main.o Auth.o UserDB.o Player.o Leaderboard.o Level.o  ScoreSystem.o Friends.o Multiplayer.o SaveGame.o MatchMaking.o AVLTree.o Inventory.o -o game \
    -L./SFML-2.5.1/lib \
    -lsfml-graphics -lsfml-window -lsfml-system

# Run if compilation successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Running game..."
    ./game
else
    echo "Compilation failed!"
fi
