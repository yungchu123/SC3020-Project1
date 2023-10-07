#include "b_plus_tree.h"
#include "types.h"
#include "memory_pool.h"

#include <iostream>
#include <string>
#include <cstddef>

using namespace std;

BPlusTree::BPlusTree(std::size_t blockSize, MemoryPool *disk)
{
    // Initialise root node of the Tree
    addressOfRootNode = new Address;
    addressOfRootNode->blockAddress = nullptr;
    addressOfRootNode->offset = blockSize;

    // Initialise root, root leads to the address of the root node of the B+ Tree.
    rootOfTree = nullptr;

    // Initialise the number of levels of B+ Tree
    levels = 0;

    // Initialise the number of nodes in the B+ Tree as 0.
    numNodes = 0;

    // Initialise node size to block size
    sizeOfNode = blockSize;

    // Size that is used to store the keys and pointers, after storing the bool isLeaf and int numOfKeys.
    nodeBufferSize = blockSize - sizeof(bool) - sizeof(int);

    size_t sizeExludingLastKeyPointer = sizeof(Address);
    maxKeys = 0;

    while(sizeExludingLastKeyPointer + sizeof(Address) + sizeof(float) <= nodeBufferSize)
    {
        sizeExludingLastKeyPointer += (sizeof(Address) + sizeof(float));
        maxKeys += 1;
    }

    if (maxKeys == 0)
    {
        throw std::overflow_error("Error: Keys and Pointers cannot fit into the Node.");
    }

}


Node::Node(int maxKeys, std::size_t blockSize)
{
    // Intialise the root of node
    // root leads to the first pointer in the node.
    addressOfNode = new Address;
    addressOfNode->blockAddress = nullptr;
    addressOfNode->offset = blockSize;
    
    // Initialise the number of keys
    numOfKeys = 0;

    // Initialise isleaf as true at first.
    isLeaf = true;

    // Array for keys
    keys = new float[maxKeys];

    // Initialise the pointers to an Address
    pointers = new void* [maxKeys + 1];

    for (int i = 0; i < maxKeys + 1; i++)
    {
        // Initialise all keys as NULL
        if (i != maxKeys)
        {
            keys[i] = NULL;
        }
    }



}