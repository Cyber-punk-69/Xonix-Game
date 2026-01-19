#include "Leaderboard.h"

// Leaderboard implemented as fixed-size min-heap (capacity 10)
Leaderboard::Leaderboard(string filename)
{
    file = filename;
    capacity = 10;
    arr = new Entry[capacity];
    size = 0;
    load();
}

Leaderboard::~Leaderboard()
{
    delete[] arr;
}

// helper: swap two entries
static void heapSwap(Leaderboard::Entry &a, Leaderboard::Entry &b)
{
    Leaderboard::Entry t = a; a = b; b = t;
}

void Leaderboard::heapifyUp(int idx)
{
    while (idx > 0)
    {
        int parent = (idx - 1) / 2;
        if (arr[idx].score >= arr[parent].score) break; // min-heap property
        heapSwap(arr[idx], arr[parent]);
        idx = parent;
    }
}

void Leaderboard::heapifyDown(int idx)
{
    while (true)
    {
        int left = idx * 2 + 1;
        int right = idx * 2 + 2;
        int smallest = idx;
        if (left < size && arr[left].score < arr[smallest].score) smallest = left;
        if (right < size && arr[right].score < arr[smallest].score) smallest = right;
        if (smallest == idx) break;
        heapSwap(arr[idx], arr[smallest]);
        idx = smallest;
    }
}

// Load scores from file and keep only top-10 using min-heap semantics
void Leaderboard::load()
{
    size = 0;
    ifstream fin(file.c_str());
    if (!fin.is_open()) return;
    string line;
    while (getline(fin, line))
    {
        if (line.empty()) continue;
        string name;
        string scoreStr;
        bool before = true;
        for (size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == '|') { before = false; continue; }
            if (before) name.push_back(line[i]); else scoreStr.push_back(line[i]);
        }
        int sc = atoi(scoreStr.c_str());
        if (size < capacity)
        {
            arr[size].name = name;
            arr[size].score = sc;
            heapifyUp(size);
            ++size;
        }
        else
        {
            if (sc > arr[0].score)
            {
                arr[0].name = name;
                arr[0].score = sc;
                heapifyDown(0);
            }
        }
    }
    fin.close();
}

void Leaderboard::save()
{
    ofstream fout(file.c_str());
    if (!fout.is_open()) return;
    for (int i = 0; i < size; ++i)
        fout << arr[i].name << "|" << arr[i].score << "\n";
    fout.close();
}

void Leaderboard::addScore(const string& name, int score)
{
    // update existing entry if present
    for (int i = 0; i < size; ++i)
    {
        if (arr[i].name == name)
        {
            if (arr[i].score == score) return; // no change
            arr[i].score = score;
            // after changing score, restore heap
            heapifyDown(i);
            heapifyUp(i);
            save();
            return;
        }
    }

    // not found: insert or replace root
    if (size < capacity)
    {
        arr[size].name = name;
        arr[size].score = score;
        heapifyUp(size);
        ++size;
        save();
        return;
    }

    if (score > arr[0].score)
    {
        arr[0].name = name;
        arr[0].score = score;
        heapifyDown(0);
        save();
    }
}

bool Leaderboard::isNewHighScore(int score)
{
    if (size < capacity) return true;
    return score > arr[0].score;
}

int Leaderboard::count() const { return size; }

bool Leaderboard::getEntry(int index, string& name, int& score)
{
    if (index < 0 || index >= size) return false;
    // copy heap and selection sort descending for display
    Entry tmp[16];
    for (int i = 0; i < size; ++i) tmp[i] = arr[i];
    for (int i = 0; i < size - 1; ++i)
    {
        int best = i;
        for (int j = i + 1; j < size; ++j)
            if (tmp[j].score > tmp[best].score) best = j;
        if (best != i) heapSwap(tmp[i], tmp[best]);
    }
    name = tmp[index].name;
    score = tmp[index].score;
    return true;
}
