#ifndef B_PLUS_TREE_H;
#define B_PLUS_TREE_H;

#include "types.h"
#include "memory_pool.h"
#include"b_plus_tree_node.h"

#include <cstddef>
#include <array>

using namespace std;

class BPlusTree
{
    private:
        // Address* addressOfRootNode; // Address of the root node
        BPlusTreeNode* rootOfTree; // Pointer to root's address in the Main Memory
        int levels; // The number of levels in the B Plus Tree
        int numNodes; // The number of nodes in the B Plus Tree
        size_t sizeOfNode; // The size of a Node in the B Plus Tree
        size_t nodeBufferSize; // The remaining space left in a node to store keys and pointers after accouting for the isLeaf bool and numOfKeys int
        int maxKeys; // The maximum number of keys that can be stored in the nod
        MemoryPool* disk; // The disk that contains the data
        void *parentDiskAddress;
        void *cursorDiskAddress;


    public:
        BPlusTree(std::size_t blockSize, MemoryPool *disk); // Constructor class for BPlusTree
        void search(float lowerBoundKey, float upperBoundKey); // search function, taking in lowerBoundKey and upperBoundKey
        void displayNode(BPlusTreeNode* Node);
        void displayTree(BPlusTreeNode* node, int level);
        int remove(double minValue, double maxValue);
        void removeInternal(float key, Node *cursorDiskAddress, Node *childDiskAddress);
        void removeLL(Address LLHeadAddress);
        void borrowOrMerge(Node *cursor, Node *parent, int leftSibling, int rightSibling);
    
};

#endif