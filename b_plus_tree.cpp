#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
#include "linked_list_node.h"
#include "types.h"

#include <cmath>
#include <iostream>

BPlusTree::BPlusTree(std::size_t blockSize, MemoryPool *disk)
{
    // Get size left for keys and pointers in a node after accounting for node's isLeaf and numKeys attributes.
    size_t nodeBufferSize = blockSize - sizeof(bool) - sizeof(int);

    // Set max keys available in a node. Each key is a float, each pointer is a struct of {void *blockAddress, short int offset}.
    // Therefore, each key is 4 bytes. Each pointer is around 16 bytes.

    // Initialize node buffer with a pointer. P | K | P , always one more pointer than keys.
    size_t sum = sizeof(Address);
    maxKeys = 0;

    // Try to fit as many pointer key pairs as possible into the node block.
    while (sum + sizeof(Address) + sizeof(float) <= nodeBufferSize)
    {
        sum += (sizeof(Address) + sizeof(float));
        maxKeys += 1;
    }

    if (maxKeys == 0)
    {
        throw std::overflow_error("Error: Keys and pointers too large to fit into a node!");
    }

    // Initialize root to NULL
    rootNode = nullptr;

    // Set node size to be equal to block size.
    nodeSize = blockSize;

    // Initialize initial variables
    levels = 0;
    numNodes = 0;

    // Initialize disk space
    this->disk = disk;
}