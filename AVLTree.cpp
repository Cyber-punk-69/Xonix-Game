#include "AVLTree.h"
#include <cstring>

AVLTree::AVLTree() : root(nullptr), nodeCount(0) {}

AVLTree::~AVLTree() {
    clear();
}

int AVLTree::getHeight(AVLNode *node) const {
    if (node == nullptr) return 0;
    return node->height;
}

int AVLTree::getBalanceFactor(AVLNode *node) const {
    if (node == nullptr) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

int AVLTree::compareStrings(const std::string &a, const std::string &b) const {
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

AVLNode *AVLTree::rotateRight(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;
    
    // Perform rotation
    x->right = y;
    y->left = T2;
    
    // Update heights
    y->height = (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right)) + 1;
    x->height = (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right)) + 1;
    
    return x;
}

AVLNode *AVLTree::rotateLeft(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;
    
    // Perform rotation
    y->left = x;
    x->right = T2;
    
    // Update heights
    x->height = (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right)) + 1;
    y->height = (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right)) + 1;
    
    return y;
}

AVLNode *AVLTree::insertHelper(AVLNode *node, const Theme &theme) {
    // Perform normal BST insertion
    if (node == nullptr) {
        nodeCount++;
        return new AVLNode(theme);
    }
    
    int cmp = compareStrings(theme.themeId, node->data.themeId);
    
    if (cmp < 0) {
        node->left = insertHelper(node->left, theme);
    } else if (cmp > 0) {
        node->right = insertHelper(node->right, theme);
    } else {
        // Duplicate key - update the theme data
        node->data = theme;
        return node;
    }
    
    // Update height of this ancestor node
    node->height = (getHeight(node->left) > getHeight(node->right) ? getHeight(node->left) : getHeight(node->right)) + 1;
    
    // Get balance factor to check if node became unbalanced
    int balance = getBalanceFactor(node);
    
    // Left Left Case
    if (balance > 1 && compareStrings(theme.themeId, node->left->data.themeId) < 0) {
        return rotateRight(node);
    }
    
    // Right Right Case
    if (balance < -1 && compareStrings(theme.themeId, node->right->data.themeId) > 0) {
        return rotateLeft(node);
    }
    
    // Left Right Case
    if (balance > 1 && compareStrings(theme.themeId, node->left->data.themeId) > 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    
    // Right Left Case
    if (balance < -1 && compareStrings(theme.themeId, node->right->data.themeId) < 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    
    return node;
}

void AVLTree::insert(const Theme &theme) {
    root = insertHelper(root, theme);
}

bool AVLTree::searchHelper(AVLNode *node, const std::string &themeId, Theme &out) const {
    if (node == nullptr) {
        return false;
    }
    
    int cmp = compareStrings(themeId, node->data.themeId);
    
    if (cmp == 0) {
        out = node->data;
        return true;
    } else if (cmp < 0) {
        return searchHelper(node->left, themeId, out);
    } else {
        return searchHelper(node->right, themeId, out);
    }
}

bool AVLTree::search(const std::string &themeId, Theme &out) const {
    return searchHelper(root, themeId, out);
}

void AVLTree::inOrderHelper(AVLNode *node, Theme *arr, int &index, int maxSize) const {
    if (node == nullptr || index >= maxSize) {
        return;
    }
    
    // Traverse left subtree
    inOrderHelper(node->left, arr, index, maxSize);
    
    // Visit node
    if (index < maxSize) {
        arr[index] = node->data;
        index++;
    }
    
    // Traverse right subtree
    inOrderHelper(node->right, arr, index, maxSize);
}

int AVLTree::inOrderTraversal(Theme *arr, int maxSize) const {
    int index = 0;
    inOrderHelper(root, arr, index, maxSize);
    return index;
}

int AVLTree::size() const {
    return nodeCount;
}

bool AVLTree::isEmpty() const {
    return root == nullptr;
}

void AVLTree::clearHelper(AVLNode *node) {
    if (node == nullptr) return;
    
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
}

void AVLTree::clear() {
    clearHelper(root);
    root = nullptr;
    nodeCount = 0;
}

