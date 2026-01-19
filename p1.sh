#!/bin/bash
# build_and_run.sh

# Set SFML path
export LD_LIBRARY_PATH="$PWD/SFML-2.5.1/lib:$LD_LIBRARY_PATH"

# Clean old object files
rm -f *.o game

# Compile all source files
echo "Compiling source files..."
g++ -c main.cpp Auth.cpp UserDB.cpp Level.cpp Player.cpp Multiplayer.cpp Leaderboard.cpp Friends.cpp ScoreSystem.cpp MatchMaking.cpp \
    -I./SFML-2.5.1/include

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed! Fix the errors above."
    exit 1
fi

# Link everything
echo "Linking..."
g++ main.o Auth.o UserDB.o Player.o Leaderboard.o Level.o ScoreSystem.o Friends.o Multiplayer.o MatchMaking.o -o game \
    -L./SFML-2.5.1/lib \
    -lsfml-graphics -lsfml-window -lsfml-system

# Check if linking was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Running game..."
    ./game
else
    echo "Linking failed!"
    exit 1
fi