#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

// ----------------------------------------------
// LevelInfo aik structure hai jo ek level ki 
// tamam basic information store karta hai.
// ----------------------------------------------
struct LevelInfo {
    int id;           // level ka number (1 se n tak)
    int enemyCount;   // is level mein kitne enemies spawn honge
    float delay;      // tile movement delay (chhota value = zyada speed)
    string name;      // level ka naam (friendly display name)
};

class LevelManager {
private:
    LevelInfo levels[5];   // total 5 levels ka data store karta hai
    int countLevels;       // kitne levels available hain
    string cfgFile;        // config file ka naam jahan last level save hota hai

    // ----------------------------------------------
    // Ye function default levels set karta hai jab 
    // config load na ho ya file missing ho.
    // ----------------------------------------------
    void initDefaultLevels();

public:

    // ----------------------------------------------
    // Constructor: jab object banega to config file 
    // ka naam set hoga aur levels initialize honge.
    // ----------------------------------------------
    LevelManager(const string& cfg = "level.cfg");

    // ----------------------------------------------
    // Destructor: object destroy hone par chalega.
    // Filhaal koi special kaam nahi karta.
    // ----------------------------------------------
    ~LevelManager();

    // ----------------------------------------------
    // Ye function kisi specific level ka LevelInfo 
    // return karta hai (1-based index).
    // Agar level out-of-range ho to level 1 return karega.
    // ----------------------------------------------
    LevelInfo getLevel(int level) const;

    // ----------------------------------------------
    // Config file se last selected level load karta hai.
    // Return 1..countLevels hota hai.
    // Agar file missing ho to default level 1 deta hai.
    // ----------------------------------------------
    int loadLastSelected() const;

    // ----------------------------------------------
    // Ye function last selected level ko config file 
    // mein save karta hai (simple integer write hota hai).
    // ----------------------------------------------
    void saveLastSelected(int level) const;

    // ----------------------------------------------
    // Total levels kitne hain, uska number return karta hai.
    // ----------------------------------------------
    int getCount() const;
};

#endif // LEVEL_H
