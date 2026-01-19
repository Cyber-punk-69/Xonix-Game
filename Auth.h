#ifndef AUTH_H
#define AUTH_H

#include "UserDB.h"
#include <string>
using namespace std;

class Auth {
private:
    UserDB db;

    string getCurrentTimestamp() const;

public:
    Auth(string file = "users.txt");

    string hashPassword(const string& pass) const;
    bool validUsername(const string& username) const;
    bool validPassword(const string& pass) const;

    bool registerUser(const string& user, const string& pass, const string& nick);
    const Player* loginUser(const string& user, const string& pass);
};

#endif
