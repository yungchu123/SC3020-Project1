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
    for (int i = 0; i < maxKeys; i++){
        keys[i] = '\0';
    }

    // Initialise the pointers to an Address
    pointers = new void* [maxKeys + 1];
    for (int i = 0; i <= maxKeys; i++){
        pointers[i] = nullptr;
    }
}
