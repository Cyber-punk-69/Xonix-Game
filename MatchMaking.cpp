#include "MatchMaking.h"

// ---------------- WaitingQueue ----------------
WaitingQueue::WaitingQueue(int cap)
    : capacity(cap), head(0), tail(0), count(0)
{
    if (capacity < 4) capacity = 4;
    data = new MatchRequest[capacity];
}

WaitingQueue::~WaitingQueue()
{
    delete[] data;
}

bool WaitingQueue::enqueue(const MatchRequest& r)
{
    if (count == capacity)
    {
        // full - simple policy: drop oldest (dequeue) and push new
        MatchRequest tmp;
        dequeue(tmp);
    }
    data[tail] = r;
    tail = (tail + 1) % capacity;
    ++count;
    return true;
}

bool WaitingQueue::dequeue(MatchRequest& out)
{
    if (count == 0) return false;
    out = data[head];
    head = (head + 1) % capacity;
    --count;
    return true;
}

bool WaitingQueue::isEmpty() const { return count == 0; }
int WaitingQueue::size() const { return count; }

void WaitingQueue::clear()
{
    head = tail = count = 0;
}

// ---------------- PriorityQueue (max-heap) ----------------
PriorityQueue::PriorityQueue(int cap)
    : capacity(cap), heapSize(0)
{
    if (capacity < 4) capacity = 4;
    heap = new MatchRequest[capacity];
}

PriorityQueue::~PriorityQueue()
{
    delete[] heap;
}

void PriorityQueue::heapifyUp(int idx)
{
    while (idx > 0)
    {
        int parent = (idx - 1) / 2;
        if (heap[idx].score <= heap[parent].score) break;
        MatchRequest tmp = heap[idx];
        heap[idx] = heap[parent];
        heap[parent] = tmp;
        idx = parent;
    }
}

void PriorityQueue::heapifyDown(int idx)
{
    while (true)
    {
        int left = idx * 2 + 1;
        int right = idx * 2 + 2;
        int largest = idx;
        if (left < heapSize && heap[left].score > heap[largest].score) largest = left;
        if (right < heapSize && heap[right].score > heap[largest].score) largest = right;
        if (largest == idx) break;
        MatchRequest tmp = heap[idx];
        heap[idx] = heap[largest];
        heap[largest] = tmp;
        idx = largest;
    }
}

bool PriorityQueue::insert(const MatchRequest& r)
{
    if (heapSize == capacity)
    {
        // no dynamic resizing per constraints; drop lowest-priority to keep room
        // find index of smallest (linear scan)
        int minIdx = 0;
        for (int i = 1; i < heapSize; ++i)
            if (heap[i].score < heap[minIdx].score) minIdx = i;
        // replace minIdx with new item and heapify
        heap[minIdx] = r;
        heapifyUp(minIdx);
        heapifyDown(minIdx);
        return true;
    }
    heap[heapSize] = r;
    heapifyUp(heapSize);
    ++heapSize;
    return true;
}

bool PriorityQueue::popMax(MatchRequest& out)
{
    if (heapSize == 0) return false;
    out = heap[0];
    heap[0] = heap[heapSize - 1];
    --heapSize;
    heapifyDown(0);
    return true;
}

bool PriorityQueue::peekMax(MatchRequest& out) const
{
    if (heapSize == 0) return false;
    out = heap[0];
    return true;
}

int PriorityQueue::size() const { return heapSize; }
bool PriorityQueue::isEmpty() const { return heapSize == 0; }

void PriorityQueue::clear()
{
    heapSize = 0;
}

// ---------------- Matchmaking ----------------
Matchmaking::Matchmaking(int waitCap, int pqCap)
    : waiting(waitCap), pq(pqCap), nextRoomId(1)
{
}

Matchmaking::~Matchmaking()
{
}

void Matchmaking::flushWaitingToPQ()
{
    MatchRequest r;
    while (waiting.dequeue(r))
    {
        pq.insert(r);
    }
}

bool Matchmaking::enqueuePlayer(const string& username, int score)
{
    MatchRequest r(username, score);
    return waiting.enqueue(r);
}

bool Matchmaking::matchNextPair(GameRoom& outRoom)
{
    // Move any waiting players to priority queue
    flushWaitingToPQ();

    // Need at least two in priority queue
    if (pq.size() < 2) return false;

    MatchRequest a, b;
    if (!pq.popMax(a)) return false;
    if (!pq.popMax(b)) {
        // push 'a' back if single pop
        pq.insert(a);
        return false;
    }

    outRoom = GameRoom(a, b, nextRoomId++);
    return true;
}

int Matchmaking::matchAll(GameRoom* outRooms, int maxRooms)
{
    if (!outRooms || maxRooms <= 0) return 0;
    flushWaitingToPQ();
    int created = 0;
    GameRoom gr;
    while (created < maxRooms && pq.size() >= 2)
    {
        MatchRequest a, b;
        pq.popMax(a);
        pq.popMax(b);
        outRooms[created] = GameRoom(a, b, nextRoomId++);
        ++created;
    }
    return created;
}

int Matchmaking::pendingPlayers() const
{
    return waiting.size() + pq.size();
}

void Matchmaking::reset()
{
    waiting.clear();
    pq.clear();
}
