#include "UserDB.h"

// Constructor - Database initialize karta hai
UserDB::UserDB(string file)
{
    storageFile = file;  // File ka naam store karo
    size = 0;            // Abhi koi players nahi hain
    capacity = 5;        // Shuruati capacity set karo
    players = new Player[capacity];  // Dynamic array banayo
    load();              // File se data load karo
}

// Destructor - Memory clean karta hai
UserDB::~UserDB() {
    delete[] players;    // Dynamic array ki memory release karo
}

// Array ka size badhata hai
void UserDB::resize() {
    capacity *= 2;       // Capacity double karo
    Player* newArr = new Player[capacity];  // Naya bara array banayo
    
    // Purane data ko naye array mein copy karo
    for (int i = 0; i < size; i++)
        newArr[i] = players[i];
    
    delete[] players;    // Purana array delete karo
    players = newArr;    // Naye array ko point karo
}

// File se data load karta hai
bool UserDB::load() {
    ifstream fin(storageFile);  // File open karo
    if (!fin.is_open()) return false;  // File nahi khuli to false return karo

    string line;
    size = 0;  // Size reset karo

    // Har line read karo
    while (getline(fin, line)) {
        if (line == "") continue;  // Khali line skip karo

        // Agar jagah nahi hai to resize karo
        if (size == capacity) resize();
        
        // Line ko Player object mein convert karo aur array mein add karo
        players[size++] = Player::deserialize(line);
    }
    fin.close();  // File close karo
    return true;
}

// Data ko file mein save karta hai
bool UserDB::save() const {
    ofstream fout(storageFile);  // File open karo writing ke liye
    if (!fout.is_open()) return false;

    // Har player ka data file mein likho
    for (int i = 0; i < size; i++) {
        fout << players[i].serialize() << "\n";  // Player data ko string mein convert karo
    }
    fout.close();  // File close karo
    return true;
}

// Check karta hai ke username pehle se exist karta hai ya nahi
bool UserDB::usernameExists(const string& username) const {
    // Har player check karo
    for (int i = 0; i < size; i++)
        if (players[i].username == username)  // Username match ho gaya
            return true;
    return false;  // Username nahi mila
}

// Naya user add karta hai
bool UserDB::addUser(const Player& p) {
    // Agar username pehle se hai to fail karo
    if (usernameExists(p.username)) return false;

    // Agar jagah nahi hai to array bara karo
    if (size == capacity) resize();
    
    // Naya player add karo aur file save karo
    players[size++] = p;
    return save();
}

// User ko authenticate karta hai (login ke liye)
const Player* UserDB::authenticate(const string& u, const string& hash) const {
    // Har player check karo
    for (int i = 0; i < size; i++) {
        // Username aur password dono match karein
        if (players[i].username == u && players[i].passwordHash == hash)
            return &players[i];  // Player ka address return karo
    }
    return nullptr;  // Authentication fail
}
