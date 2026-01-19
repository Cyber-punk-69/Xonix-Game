#ifndef TILENODE_H
#define TILENODE_H

struct TileNode {
    int x, y;
    TileNode *next;
    TileNode(int a, int b) : x(a), y(b), next(nullptr) {}
};

#endif
