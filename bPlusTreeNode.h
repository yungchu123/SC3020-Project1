#ifndef B_PLUS_TREE_NODE_H
#define B_PLUS_TREE_NODE_H

#include "types.h"

class bPlusTreeNode
{
    friend class BPlusTree;

private:
    Address *pointers; // A pointer to an array of pointers.
    float *keys;       // A Pointer to an array of keys in this node.
    int numKeys;       // Current number of keys in this node.
    bool isLeaf;       // Node is a leaf or internal node

public:
    bPlusTreeNode(int maxKeys); // Value of n (max number of keys in a node)
};

#endif