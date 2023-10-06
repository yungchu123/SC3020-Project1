#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include "memory_pool.h"
#include "b_plus_tree_node.h"
#include "types.h"

class BPlusTree
{
private:
    /* data */
    MemoryPool *disk;        // Pointer to a memory pool for data blocks.
    BPlusTreeNode *rootNode; // Pointer to the main memory root
    int maxKeys;             // Maximum keys in a node.
    int levels;              // Number of levels in this B+ Tree.
    int numNodes;            // Number of nodes in this B+ Tree.
    std::size_t nodeSize;    // Size of a node = Size of block.
public:
    // Constructor
    BPlusTree(std::size_t blockSize, MemoryPool *disk);

    // Inserting a new key
    void insert(Address *address, float key);

    // Shifting layer when inserting new records (if necessary)
    void restructureTree(int x, BPlusTreeNode *parentNode, BPlusTreeNode *childNode);

    // Locate parent node
    BPlusTreeNode *findParent(BPlusTreeNode *current, BPlusTreeNode *child);
};

#endif