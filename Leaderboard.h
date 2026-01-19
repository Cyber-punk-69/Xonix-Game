#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class Leaderboard {
public:
    struct Entry {
        string name;
        int score;
    };

    Leaderboard(string filename = "leaderboard.txt");
    ~Leaderboard();

    void load();
    void save();

    void addScore(const string& name, int score);
    bool isNewHighScore(int score);

    int count() const;
    bool getEntry(int index, string& name, int& score);

private:
    Entry* arr;
    int size;
    int capacity;
    string file;

    void heapifyUp(int idx);
    void heapifyDown(int idx);
};

#endif
