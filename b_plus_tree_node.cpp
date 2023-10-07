

#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
#include "types.h"
#include "memory_pool.h"

#include <iostream>
#include <string>
#include <cstddef>

BPlusTreeNode::BPlusTreeNode(int maxKeys)
{
    
    // Initialise the number of keys
    numKeys = 0;

    // Initialise isleaf as true at first.
    isLeaf = true;

    // Array for keys
    keys = new float[maxKeys];

    // Initialise the pointers to an Address
    pointers = new void* [maxKeys + 1];

    for (int i = 0; i < maxKeys; i++)
    {
        // Initialise all keys as NULL
        keys[i] = '\0';
    }

    for (int i = 0; i < maxKeys + 1; i++)
    {
        // Initialise all pointers as NULL
        pointers[i] = nullptr;
    }
}
