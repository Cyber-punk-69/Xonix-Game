#pragma once
#include <string>
#include <cstring>
#include <cstdio>

// Simple friend system using only dynamic arrays and linked lists (no STL containers).
// - Dynamic array of PlayerEntry
// - Per-player linked list of friends
// - Per-player linked list of pending requests
// - Simple open-addressing hash table mapping username -> player index

struct FriendNode
{
    char name[64];
    FriendNode *next;
    FriendNode(const char *n)
    {
        strncpy(name, n, 63);
        name[63] = '\0';
        next = nullptr;
    }
};

struct PendingNode
{
    char name[64];
    PendingNode *next;
    PendingNode(const char *n)
    {
        strncpy(name, n, 63);
        name[63] = '\0';
        next = nullptr;
    }
};

struct PlayerEntry
{
    char username[64];
    FriendNode *friends;  // head of friends linked list
    PendingNode *pending; // head of pending requests (requests TO this player)
    PlayerEntry()
    {
        username[0] = '\0';
        friends = nullptr;
        pending = nullptr;
    }
};

class FriendSystem
{
public:
    FriendSystem(int initialCapacity = 128, int hashSize = 1024);
    ~FriendSystem();

    // ensure a player exists; returns their index
    int ensurePlayer(const std::string &username);
    // find index or -1
    int findIndex(const std::string &username) const;

    // send a friend request from->to; returns true if enqueued
    bool sendFriendRequest(const std::string &from, const std::string &to);
    // accept a pending request 'from' for player 'to'
    bool acceptFriendRequest(const std::string &to, const std::string &from);
    bool rejectFriendRequest(const std::string &to, const std::string &from);

    // persist/load friend state
    bool saveToFile(const char *path) const;
    bool loadFromFile(const char *path);
    // ensure players exist from a users DB (first token per line)
    void ensurePlayersFromFile(const char *usersPath);

    // iterate friends/pending
    int getFriendCount(const std::string &username) const;
    bool getFriendAt(const std::string &username, int idx, std::string &outName) const;

    int getPendingCount(const std::string &username) const;
    bool getPendingAt(const std::string &username, int idx, std::string &outName) const;

private:
    PlayerEntry *players;
    int count;
    int capacity;

    char **hashKeys; // hash table keys (C strings) or nullptr
    int *hashVals;   // mapped player index
    int tableSize;

    int hash(const char *s) const;
    void grow();
    void addFriendByIndex(int aIdx, int bIdx);
    bool isAlreadyFriends(int aIdx, const char *bname) const;
    bool pendingExists(int idx, const char *from) const;
    void freeListNodes();
};
