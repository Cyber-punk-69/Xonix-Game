// SaveGame.h
// Implements save / load of an in-progress game using a linked-list of tiles.
#ifndef SAVEGAME_H
#define SAVEGAME_H
#include"TileNode.h"
#include <string>
#include <ctime>



class SaveGame {
public:
    SaveGame(const std::string &saveId = "", const std::string &user = "");
    ~SaveGame();

    // linked-list helpers
    void addTile(int x, int y);
    void clearTiles();
    int tileCount() const;

    // fields that describe the saved state
    std::string saveId;       // unique id associated with this save
    std::string username;     // player id
    std::time_t timestamp;    // save time

    // minimal in-game state to resume (optional but useful)
    int playerX;
    int playerY;
    int playerDX;
    int playerDY;
    
    // score system state
    int currentScore;
    int powerupInventory;
    int rewardCount;

    // persistence
    bool saveToFile(const std::string &dir = "saves") const;
    static bool loadFromFile(const std::string &saveId, SaveGame &out, const std::string &dir = "saves");

    // generate a reasonably-unique save id (username + timestamp + random)
    static std::string generateSaveId(const std::string &username);

private:
    TileNode *tiles;

public:
    // Accessor to iterate tiles after load
    TileNode *getTiles() const { return tiles; }

    // ensure directory exists (cross-platform)
    static void ensureDirExists(const std::string &d);

};

#endif // SAVEGAME_H
