#include "Auth.h"
#include <ctime>

// Constructor - UserDB ko file name pass karta hai
Auth::Auth(string file) : db(file) {}

// Current date aur time string mein return karta hai
string Auth::getCurrentTimestamp() const
{
    time_t now = time(0);      // Current time lelo
    char buf[32];              // Result store karne ke liye buffer
    tm* t = localtime(&now);   // Local time mein convert karo

    // Format: YYYY-MM-DD HH:MM:SS
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);

    return string(buf);        // String mein return karo
}

// Password ko hash karta hai (secure storage ke liye)
string Auth::hashPassword(const string& pass) const 
{
    unsigned long hash = 5381;  // djb2 algorithm ka initial value
    
    // Har character ko hash mein process karo
    for (int i = 0; i < (int)pass.length(); i++)
        hash = ((hash << 5) + hash) + pass[i]; // djb2 algorithm
    
    return to_string(hash);     // Hash ko string mein convert karo
}

// Username valid hai ya nahi check karta hai
bool Auth::validUsername(const string& username) const 
{
    // Username kam se kam 3 characters ka hona chahiye
    // Aur usme '|' character nahi hona chahiye (kyunki yeh separator hai)
    return username.length() >= 3 && username.find('|') == string::npos;
}

// Password valid hai ya nahi check karta hai
bool Auth::validPassword(const string& pass) const
{
    // Password kam se kam 4 characters ka hona chahiye
    return pass.length() >= 4;
}

// Naya user register karta hai
bool Auth::registerUser(const string& user, const string& pass, const string& nick) 
{
    // Pehle username aur password validate karo
    if (!validUsername(user) || !validPassword(pass))
        return false;  // Invalid data hai to fail karo

    // Check karo ke username pehle se to nahi hai
    if (db.usernameExists(user))
        return false;  // Username already exists

    // Naya Player object banao with hashed password aur current timestamp
    Player p(user, hashPassword(pass), nick, getCurrentTimestamp());

    // Database mein user add karo (yeh automatically save bhi karega)
    return db.addUser(p);
}

// User login karta hai
const Player* Auth::loginUser(const string& user, const string& pass)
{
    // Password ko hash karo aur database se authenticate karo
    return db.authenticate(user, hashPassword(pass));
}
