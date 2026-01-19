#ifndef AVLTREE_H
#define AVLTREE_H

#include "Theme.h"

// AVL Tree Node structure
struct AVLNode {
    Theme data;
    AVLNode *left;
    AVLNode *right;
    int height;
    
    AVLNode(const Theme &theme) 
        : data(theme), left(nullptr), right(nullptr), height(1) {}
};

// AVL Tree class for storing themes
// Key: themeId (string)
class AVLTree {
public:
    AVLTree();
    ~AVLTree();
    
    // Insert a theme into the tree
    void insert(const Theme &theme);
    
    // Search for a theme by ID
    bool search(const std::string &themeId, Theme &out) const;
    
    // Get all themes in sorted order (in-order traversal)
    // Returns number of themes stored in the provided array
    int inOrderTraversal(Theme *arr, int maxSize) const;
    
    // Get the number of nodes in the tree
    int size() const;
    
    // Check if tree is empty
    bool isEmpty() const;
    
    // Clear the tree
    void clear();

private:
    AVLNode *root;
    int nodeCount;
    
    // Helper functions
    int getHeight(AVLNode *node) const;
    int getBalanceFactor(AVLNode *node) const;
    AVLNode *rotateRight(AVLNode *y);
    AVLNode *rotateLeft(AVLNode *x);
    AVLNode *insertHelper(AVLNode *node, const Theme &theme);
    bool searchHelper(AVLNode *node, const std::string &themeId, Theme &out) const;
    void inOrderHelper(AVLNode *node, Theme *arr, int &index, int maxSize) const;
    void clearHelper(AVLNode *node);
    int compareStrings(const std::string &a, const std::string &b) const; // returns -1, 0, or 1
};

#endif // AVLTREE_H

