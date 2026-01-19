// SaveGame.cpp
// FIXED VERSION: Properly saves and loads all game state
#include "SaveGame.h"
#include <fstream>
#include <ctime>
#include <string>

SaveGame::SaveGame(const std::string &saveId_, const std::string &user)
    : saveId(saveId_), username(user), timestamp(std::time(nullptr)),
      playerX(0), playerY(0), playerDX(0), playerDY(0),
      currentScore(0), powerupInventory(0), rewardCount(0), tiles(nullptr)
{
}

SaveGame::~SaveGame()
{
    clearTiles();
}

void SaveGame::addTile(int x, int y)
{
    TileNode *n = new TileNode(x, y);
    n->next = tiles;
    tiles = n;
}

void SaveGame::clearTiles()
{
    TileNode *p = tiles;
    while (p)
    {
        TileNode *n = p->next;
        delete p;
        p = n;
    }
    tiles = nullptr;
}

int SaveGame::tileCount() const
{
    int c = 0;
    for (TileNode *p = tiles; p; p = p->next)
        ++c;
    return c;
}

void SaveGame::ensureDirExists(const std::string & /*d*/)
{
    // No-op: avoid using platform-specific or <filesystem> APIs per request.
    // Assumes the `saves/` directory already exists in the project workspace.
    // For production: use mkdir on Unix or _mkdir on Windows
}

bool SaveGame::saveToFile(const std::string &dir) const
{
    if (saveId.empty())
        return false;

    // Not creating directories here; caller/project should ensure `dir` exists.
    std::string path = dir + "/" + saveId + ".sav";
    std::ofstream fout(path.c_str());
    if (!fout.is_open())
        return false;

    // Enhanced format (text) with complete game state:
    // Line 1: saveId
    // Line 2: username
    // Line 3: timestamp (as integer)
    // Line 4: playerX playerY playerDX playerDY
    // Line 5: currentScore powerupInventory rewardCount
    // Line 6: tileCount
    // Following lines: x y per tile

    fout << saveId << "\n";
    fout << username << "\n";
    fout << (long long)timestamp << "\n";
    fout << playerX << " " << playerY << " " << playerDX << " " << playerDY << "\n";
    fout << currentScore << " " << powerupInventory << " " << rewardCount << "\n";

    int cnt = tileCount();
    fout << cnt << "\n";

    for (TileNode *p = tiles; p; p = p->next)
    {
        fout << p->x << " " << p->y << "\n";
    }

    fout.close();
    return true;
}

bool SaveGame::loadFromFile(const std::string &saveId_, SaveGame &out, const std::string &dir)
{
    if (saveId_.empty())
        return false;

    std::string path = dir + "/" + saveId_ + ".sav";
    std::ifstream fin(path.c_str());
    if (!fin.is_open())
        return false;

    std::string line;

    // Line 1: saveId
    if (!std::getline(fin, line))
    {
        fin.close();
        return false;
    }
    out.saveId = line;

    // Line 2: username
    if (!std::getline(fin, line))
    {
        fin.close();
        return false;
    }
    out.username = line;

    // Line 3: timestamp
    long long ts = 0;
    if (!(fin >> ts))
    {
        fin.close();
        return false;
    }
    out.timestamp = (std::time_t)ts;
    std::getline(fin, line); // consume remainder of line

    // Line 4: player position/direction
    if (!(fin >> out.playerX >> out.playerY >> out.playerDX >> out.playerDY))
    {
        fin.close();
        return false;
    }
    std::getline(fin, line); // consume remainder

    // Line 5: score, powerups, rewards (NEW - handle both old and new format)
    // Try to read these values; if they don't exist (old format), default to 0
    int score = 0, inventory = 0, rewards = 0;
    if (fin >> score >> inventory >> rewards)
    {
        out.currentScore = score;
        out.powerupInventory = inventory;
        out.rewardCount = rewards;
        std::getline(fin, line); // consume remainder
    }
    else
    {
        // Old format file - just clear the error and continue
        fin.clear();
        out.currentScore = 0;
        out.powerupInventory = 0;
        out.rewardCount = 0;
    }

    // Line 6: tile count
    int cnt = 0;
    if (!(fin >> cnt))
    {
        fin.close();
        return false;
    }
    std::getline(fin, line);

    // Clear existing tiles and load saved tiles
    out.clearTiles();
    for (int i = 0; i < cnt; ++i)
    {
        int x = 0, y = 0;
        if (!(fin >> x >> y))
        {
            fin.close();
            return false;
        }
        out.addTile(x, y);
        std::getline(fin, line);
    }

    fin.close();
    return true;
}

std::string SaveGame::generateSaveId(const std::string &username)
{
    // Build id using timestamp + an incrementing counter to avoid <random> and <sstream>
    static int counter = 0;
    ++counter;
    std::time_t now = std::time(nullptr);
    std::string id = username + "_" + std::to_string((long long)now) + "_" + std::to_string(counter);
    return id;
}