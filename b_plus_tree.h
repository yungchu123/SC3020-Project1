#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include "types.h"
#include "memory_pool.h"

#include <cstddef>
#include <array>

class Node
{
    private:
        Address* addressOfNode; // Address of the node
        int numOfKeys; // The current number of keys in the node
        bool isLeaf; // an indicator that indicates whether it is a leaf node or not
        float *keys; // Pointer to an array of keys that the Node holds
        Address *pointers; // Pointer to an array of Address structs for other nodes it is pointing to


    public:
        Node(int maxKeys, std::size_t blockSize); // Constructor class for Node

        bool getIsLeaf(){
            return isLeaf;
        }

        int getNumOfKeys(){
            return numOfKeys;
        }

        float* getKeys(){
            return keys;
        }

        Address* getPointers(){
            return pointers;
        }
};

class BPlusTree
{
    private:
        Address* addressOfRootNode; // Address of the root node
        Node *rootOfTree; // Pointer to root's address in the Main Memory
        int levels; // The number of levels in the B Plus Tree
        int numNodes; // The number of nodes in the B Plus Tree
        size_t sizeOfNode; // The size of a Node in the B Plus Tree
        size_t nodeBufferSize;
        int maxKeys;


    public:
        BPlusTree(std::size_t blockSize, MemoryPool *disk);
        void search(float lowerBoundKey, float upperBoundKey);
    
};



#endif