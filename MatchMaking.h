#ifndef MATCHMAKING_H
#define MATCHMAKING_H

#include <iostream>
#include <string>
#include <ctime>
#include <cstring>
#include <fstream>

using namespace std;

// Simple match request representing a player waiting for a match
struct MatchRequest {
    string username; // player id (may be empty for guest)
    int score;       // player's total score used as priority (higher = higher priority)

    MatchRequest() : username(""), score(0) {}
    MatchRequest(const string& u, int s) : username(u), score(s) {}
};

// A small game room holding two players
struct GameRoom {
    MatchRequest p1;
    MatchRequest p2;
    int roomId;
    bool active;

    GameRoom() : p1(), p2(), roomId(0), active(false) {}
    GameRoom(const MatchRequest& a, const MatchRequest& b, int id)
        : p1(a), p2(b), roomId(id), active(true) {}
};

// WaitingQueue: simple FIFO circular buffer (no STL)
class WaitingQueue {
private:
    MatchRequest* data;
    int capacity;
    int head;
    int tail;
    int count;

public:
    WaitingQueue(int cap = 128);
    ~WaitingQueue();

    bool enqueue(const MatchRequest& r);
    bool dequeue(MatchRequest& out);
    bool isEmpty() const;
    int size() const;
    void clear();
};

// PriorityQueue: binary max-heap based on score
class PriorityQueue {
private:
    MatchRequest* heap; // 0..(capacity-1)
    int capacity;
    int heapSize;

    void heapifyUp(int idx);
    void heapifyDown(int idx);

public:
    PriorityQueue(int cap = 256);
    ~PriorityQueue();

    bool insert(const MatchRequest& r);
    bool popMax(MatchRequest& out);
    bool peekMax(MatchRequest& out) const;
    int size() const;
    bool isEmpty() const;
    void clear();
};

// Matchmaking system: manages a waiting queue and priority queue; pairs top two players
class Matchmaking {
private:
    WaitingQueue waiting;
    PriorityQueue pq;
    int nextRoomId;

    // move all waiting requests into priority queue (caller: internal)
    void flushWaitingToPQ();

public:
    Matchmaking(int waitCap = 128, int pqCap = 256);
    ~Matchmaking();

    // enqueue a player into the waiting FIFO; score must be provided by caller
    bool enqueuePlayer(const string& username, int score);

    // Try to create one match. Returns true and fills outRoom if a pair was formed.
    // This will move waiting entries into the internal priority queue first.
    bool matchNextPair(GameRoom& outRoom);

    // Create as many matches as possible and write them into provided array up to maxRooms.
    // Returns the number of rooms created.
    int matchAll(GameRoom* outRooms, int maxRooms);

    // number of pending players (waiting + in priority queue)
    int pendingPlayers() const;

    // clear all queues
    void reset();
};

#endif // MATCHMAKING_H
