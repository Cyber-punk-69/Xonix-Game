#include "Player.h"

Player::Player() {}

Player::Player(string u, string h, string n, string ts) : username(u), passwordHash(h), nickname(n), registrationTimestamp(ts) {} //used initializer list

string Player::serialize() const 
{
    return username + "|" + passwordHash + "|" + nickname + "|" + registrationTimestamp;
}

Player Player::deserialize(const string& line)
{
    Player p;                    // Naya player banaya
    string temp = "";            // Temporary string
    int field = 0;               // Yeh batayega ke ab konsa field process ho raha hai

    for (int i = 0; i < (int)line.size(); i++)
    {
        if (line[i] == '|')      // Agar '|' mile to field khatam ho gaya
        {
            if (field == 0)      // Pehla field - username
                p.username = temp;
            else if (field == 1) // Dusra field - password hash
                p.passwordHash = temp;
            else if (field == 2) // Teesra field - nickname
                p.nickname = temp;
            
            temp = "";           // Temp ko reset karo next field ke liye
            field++;             // Agle field pe chale jao
        } else {
            temp += line[i];     // Character ko temp mein add karo
        }
    }

    p.registrationTimestamp = temp;  // Aakhri field - registration time
    return p;                    // Complete player object return karo
}

PlayerState::PlayerState()
{
    x = y = 0;
    dx = dy = 0;
    alive = true;
    constructing = false;
    username = "";
    nickname = "";
    invIndex = 0;
}

void PlayerState::reset(int startX, int startY)
{
    x = startX; y = startY;
    dx = dy = 0;
    alive = true;
    constructing = false;
}
