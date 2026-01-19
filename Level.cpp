#include "Level.h"

LevelManager::LevelManager(const string& cfg) : cfgFile(cfg)
{
    // ----------------------------------------------
    // Constructor: yahan par total levels set kiye jaate hain
    // aur default level settings initialize hoti hain.
    // ----------------------------------------------
    countLevels = 5;
    initDefaultLevels();
}

LevelManager::~LevelManager() 
{
    // ----------------------------------------------
    // Destructor: koi dynamic memory use nahi hui,
    // is liye yahan kuch free karne ki zarurat nahi.
    // ----------------------------------------------
}

void LevelManager::initDefaultLevels() {
    // ----------------------------------------------
    // Ye method har level ka default data manually set karta hai.
    // Har level ki difficulty, enemy count aur speed alag hoti hai.
    // ----------------------------------------------

    // Level 1: easy level — kam enemies aur slow speed
    levels[0].id = 1; 
    levels[0].enemyCount = 2; 
    levels[0].delay = 0.12f; 
    levels[0].name = "Easy";

    // Level 2: thoda normal difficulty
    levels[1].id = 2; 
    levels[1].enemyCount = 3; 
    levels[1].delay = 0.10f; 
    levels[1].name = "Normal";

    // Level 3: hard level — enemies zyada aur speed tez
    levels[2].id = 3; 
    levels[2].enemyCount = 4; 
    levels[2].delay = 0.08f; 
    levels[2].name = "Hard";

    // Level 4: very hard — bohat zyada enemies + fast movement
    levels[3].id = 4; 
    levels[3].enemyCount = 6; 
    levels[3].delay = 0.06f; 
    levels[3].name = "Very Hard";

    // Level 5: insane mode — maximum enemies + fastest speed
    levels[4].id = 5; 
    levels[4].enemyCount = 8; 
    levels[4].delay = 0.045f; 
    levels[4].name = "Insane";
}

LevelInfo LevelManager::getLevel(int level) const 
{
    // ----------------------------------------------
    // Ye function kisi bhi 1-based level ka data return karta hai.
    // Agar user galat level number de (0 ya out-of-range),
    // to safe side par level ko theek kiya jaata hai.
    // ----------------------------------------------
    if (level < 1) level = 1;
    if (level > countLevels) level = countLevels;

    return levels[level - 1];
}

int LevelManager::loadLastSelected() const
{
    // ----------------------------------------------
    // Ye function config file se last selected level padhta hai.
    // Agar file na mile to by default level 1 return karta hai.
    // ----------------------------------------------
    ifstream fin(cfgFile.c_str());
    if (!fin.is_open()) return 1;

    int l = 1;
    fin >> l;
    fin.close();

    // Galat values ko ignore karke level 1 par set kar deta hai
    if (l < 1 || l > countLevels) return 1;

    return l;
}

void LevelManager::saveLastSelected(int level) const 
{
    // ----------------------------------------------
    // Ye method last played/selected level ko config file mein
    // save karta hai taake next time game wahin se continue ho.
    // ----------------------------------------------
    ofstream fout(cfgFile.c_str());
    if (!fout.is_open()) return;

    // Safety: level ko valid range me rakhna
    if (level < 1) level = 1;
    if (level > countLevels) level = countLevels;

    fout << level << "\n";
    fout.close();
}

int LevelManager::getCount() const {
    // ----------------------------------------------
    // Total levels ka count return karta hai.
    // ----------------------------------------------
    return countLevels;
}
