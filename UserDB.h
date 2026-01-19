#ifndef USERDB_H
#define USERDB_H

#include "Player.h"
#include <string>
#include <fstream>
using namespace std;

class UserDB {
private:
    Player* players;
    int size;
    int capacity;
    string storageFile;

    void resize();

public:
    UserDB(string file = "users.txt"); //when this class is called it always open users.txt to put user credentials
    ~UserDB();

    bool load();
    bool save() const;
    bool usernameExists(const string& username) const;
    bool addUser(const Player& p);
    const Player* authenticate(const string& u, const string& hash) const;
};

#endif
