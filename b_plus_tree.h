#ifndef B_PLUS_TREE_H;
#define B_PLUS_TREE_H;

#include "types.h"
#include "memory_pool.h"

#include <cstddef>
#include <array>



class BPlusTree
{
    private:
        Address* addressOfRootNode; // Address of the root node
        BPlusTreeNode *rootOfTree; // Pointer to root's address in the Main Memory
        int levels; // The number of levels in the B Plus Tree
        int numNodes; // The number of nodes in the B Plus Tree
        size_t sizeOfNode; // The size of a Node in the B Plus Tree
        size_t nodeBufferSize; // The remaining space left in a node to store keys and pointers after accouting for the isLeaf bool and numOfKeys int
        int maxKeys; // The maximum number of keys that can be stored in the nod
        MemoryPool* disk; // The disk that contains the data


    public:
        BPlusTree(std::size_t blockSize, MemoryPool *disk); // Constructor class for BPlusTree
        void search(float lowerBoundKey, float upperBoundKey); // search function, taking in lowerBoundKey and upperBoundKey
    
};

class BPlusTreeNode
{
    friend class BPlusTree;
    
    private:
        int numOfKeys; // The current number of keys in the node
        bool isLeaf; // an indicator that indicates whether it is a leaf node or not
        float *keys; // A pointer to an array of keys that the Node holds
        void **pointers; // A pointer pointing to an array of pointers


    public:
        BPlusTreeNode(int maxKeys, std::size_t blockSize); // Constructor class for BPlusTreeNode
};

#endif