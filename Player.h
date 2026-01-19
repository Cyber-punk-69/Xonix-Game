#ifndef PLAYER_H
#define PLAYER_H

#include <string>
using namespace std;

struct PlayerState {
    int x, y;         // tile coords
    int dx, dy;       // movement direction (-1/0/1)
    bool alive;       // alive flag
    bool constructing; // whether currently laying trail
    std::string username; // optional account name
    std::string nickname; // optional display name
    int invIndex;     // index used by ScoreSystem / UI to identify this player (0 or 1)

    PlayerState();
    void reset(int startX, int startY);
};

class Player {
public:
    string username;
    string passwordHash;
    string nickname;
    string registrationTimestamp;

    Player();
    Player(string u, string h, string n, string ts); // user registration credentials are passing

    string serialize() const;                        //Data ko 1 string bna kr save krna 
    static Player deserialize(const string& line);   //string sy wapis original data nikalna ka process
};

#endif
