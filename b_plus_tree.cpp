#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
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

void BPlusTree::insert(Address *address, float key)
{
    std::cout << "Inserting Key = " << key << ", Address = " << address << std::endl;
    // B+ Tree is currently empty
    if (this->rootNode == nullptr)
    {
        BPlusTreeNode *newRoot = new BPlusTreeNode(maxKeys);
        newRoot->isLeaf = true;
        newRoot->numKeys = 1;
        newRoot->keys[0] = key;
        newRoot->pointers[0] = address;

        this->rootNode = newRoot; // Initialising B+ Tree root node
        // index->saveToDisk(newRoot, nodeSize);

    }
    // Traverse B+ Tree to find location to insert key
    else
    {
        BPlusTreeNode *current = rootNode;
        BPlusTreeNode *parent;

        // Identifying the suitable leaf node
        while (current->isLeaf == false)
        {
            parent = current;
            for (int i = 0; i < current->numKeys; i++)
            {
                if (key < current->keys[i])
                {
                    BPlusTreeNode *nextNodeAdd = (BPlusTreeNode *)current->pointers[i];
                    current = nextNodeAdd;
                    // current = (BPlusTreeNode *)index->loadFromDisk(nextNodeAdd, nodeSize);
                    break;
                }
                if (i == (current->numKeys - 1))
                {
                    BPlusTreeNode *nextNodeAdd = (BPlusTreeNode *)current->pointers[i + 1];
                    current = nextNodeAdd;
                    // current = (BPlusTreeNode *)index->loadFromDisk(nextNodeAdd, nodeSize);
                    break;
                }
            }
        }

        // Check if the node is filled or not
        // Leaf node is not filled
        if (current->numKeys < maxKeys)
        {
            BPlusTreeNode *nextNodeAdd = (BPlusTreeNode *)current->pointers[current->numKeys];
            int i = 0;
            while (key > current->keys[i] && i < current->numKeys)
            {
                i++;
            }
            for (int j = current->numKeys; j > i; j--)
            {
                current->keys[j] = current->keys[j - 1];
                current->pointers[j] = current->pointers[j - 1];
            }

            current->keys[i] = key;
            current->pointers[i] = address;

            current->numKeys++;
            current->pointers[current->numKeys] = nextNodeAdd;
        }
        // Leaf node is filled
        else
        {
            // Arranging the node in ascending order, after inserting new key
            int tempNode[maxKeys + 1];

            for (int i = 0; i < maxKeys; i++)
            {
                tempNode[i] = current->keys[i];
            }

            int newKeyIndex = 0;

            while (newKeyIndex < maxKeys)
            {
                if (key > tempNode[newKeyIndex])
                {
                    newKeyIndex++;
                }
                else
                {
                    break;
                }
            }
            for (int j = maxKeys; j > newKeyIndex; j--)
            {
                tempNode[j] = tempNode[j - 1];
            }
            tempNode[newKeyIndex] = key;

            // Creating new leaf Node
            BPlusTreeNode *newNode = new BPlusTreeNode(maxKeys);
            newNode->isLeaf = true;

            // Splitting the numKeys between both currentNode and newNode
            current->numKeys = ceil((maxKeys + 1) / 2);
            newNode->numKeys = floor((maxKeys + 1) / 2);

            // Setting the last pointer (, which points to the next node)
            newNode->pointers[newNode->numKeys] = current->pointers[maxKeys];
            current->pointers[current->numKeys] = newNode;

            // Rearranging the nodes according to the ascending order

            int currentIndex = (current->numKeys) - 1; // Maintain the index for the updated currentNode
            int currentOldIndex = maxKeys - 1;         // Maintain the original index for key/pointers in currentNode
            int newIndex = (newNode->numKeys) - 1;     // Maintain the index for the updated newNode

            for (int i = maxKeys; i >= 0; i--)
            {

                int tempKey = tempNode[i];

                if (i < ((maxKeys + 1) / 2))
                { // Will be in currentNode
                    if (tempKey == key)
                    {
                        current->keys[currentIndex] = key;
                        current->pointers[currentIndex] = address;
                        currentIndex--;
                    }
                    else
                    {
                        current->keys[currentIndex] = current->keys[currentOldIndex];
                        current->pointers[currentIndex] = current->pointers[currentOldIndex];
                        currentIndex--;
                        currentOldIndex--;
                    }
                }
                else
                { // Will be in newNode
                    if (tempKey == key)
                    {
                        newNode->keys[newIndex] = key;
                        newNode->pointers[newIndex] = address;
                        newIndex--;
                    }
                    else
                    {
                        newNode->keys[newIndex] = current->keys[currentOldIndex];
                        newNode->pointers[newIndex] = current->pointers[currentOldIndex];
                        newIndex--;
                        currentOldIndex--;
                    }
                }
            }

            // Update keys/ pointers that have been moved to newNode to NULL
            for (int i = current->numKeys; i < maxKeys; i++)
            {
                current->keys[i] = -1;
                current->pointers[i] = nullptr;
            }

            // Adjusting/ Creating parent node
            // Check if currentNode == root
            if (this->rootNode == current)
            {
                BPlusTreeNode *newRoot = new BPlusTreeNode(maxKeys);
                newRoot->numKeys = 1;
                newRoot->isLeaf = false;
                newRoot->keys[0] = newNode->keys[0];
                newRoot->pointers[0] = current;
                newRoot->pointers[1] = newNode;

                this->rootNode = newRoot;
            }
            else
            {
                restructureTree(newNode->keys[0], parent, newNode);
            }
        }
    }
}

void BPlusTree::restructureTree(int x, BPlusTreeNode *parentNode, BPlusTreeNode *childNode)
{
    int newNodeSmallestKey = x;

    if (parentNode->numKeys < maxKeys)
    { // Parent node has empty space to accomodate a new childNode
        int i = 0;
        // Getting the position to insert the new childNode
        while (newNodeSmallestKey > parentNode->keys[i] && i < parentNode->numKeys)
        {
            i++;
        }
        for (int j = parentNode->numKeys; j > i; j--)
        {
            parentNode->keys[j] = parentNode->keys[j - 1];
            parentNode->pointers[j + 1] = parentNode->pointers[j];
        }

        parentNode->keys[i] = newNodeSmallestKey;
        parentNode->pointers[i + 1] = childNode;
        parentNode->numKeys++;
    }
    else
    { // Parent Node is full --> Requires shifting of tree
        float tempKeys[maxKeys + 1];
        void *tempPointers[maxKeys + 2];

        bool flag = false;
        int indexNewKey = -1;

        // Inserting values into tempKeys
        for (int i = 0; i < maxKeys + 1; i++)
        {
            if (newNodeSmallestKey > parentNode->keys[i])
            { // When parentKey < new key
                tempKeys[i] = parentNode->keys[i];
            }
            else if (!flag) // First instance when parentKey > new Key --> this the position where the new key is to be placed
            {
                tempKeys[i] = newNodeSmallestKey;
                indexNewKey = i;
                flag = true;
            }
            else
            { // Copying the remaining keys from parentNode to tempKeys
                tempKeys[i] = parentNode->keys[i - 1];
            }
        }

        // inserting values into tempPointers
        flag = false;
        for (int i = 0; i < maxKeys + 2; i++)
        {
            if (i < indexNewKey)
            { // Copying pointers before the inserted key
                tempPointers[i] = parentNode->pointers[i];
            }
            else if (i == indexNewKey)
            { // Copying pointer just before inserted key
                tempPointers[i] = parentNode->pointers[i];
                flag = true;
            }
            else if (flag)
            { // Inserting pointer to child node
                tempPointers[i] = childNode;
                flag = false;
            }
            else
            { // Copying the remaining pointers from parentNode to tempPointers
                tempPointers[i] = parentNode->pointers[i - 1];
            }
        }

        // Creating newParentNode
        BPlusTreeNode *newParentNode = new BPlusTreeNode(maxKeys);
        newParentNode->isLeaf = false;

        // Splitting the numKeys between both parentNode and newParentNode
        parentNode->numKeys = ceil(maxKeys / 2);
        newParentNode->numKeys = floor(maxKeys / 2);

        // Rearranging the newParentNode
        for (int i = 0, j = parentNode->numKeys + 1; i < newParentNode->numKeys; i++, j++)
        {
            newParentNode->keys[i] = tempKeys[j];
        }
        for (int i = 0, j = parentNode->numKeys + 1; i < (newParentNode->numKeys + 1); i++, j++)
        {
            newParentNode->pointers[i] = tempPointers[j];
        }

        // Rearranging the parentNode
        for (int i = 0; i < maxKeys; i++)
        {
            if (i < parentNode->numKeys)
            {
                parentNode->keys[i] = tempKeys[i];
            }
            else
            {
                parentNode->keys[i] = -1;
            }
        }
        for (int i = 0; i < maxKeys + 1; i++)
        {
            if (i < (parentNode->numKeys + 1))
            {
                parentNode->pointers[i] = tempPointers[i];
            }
            else
            {
                parentNode->pointers[i] = nullptr;
            }
        }

        // Adjusting/ Creating parent node
        // Check if currentNode == root
        if (this->rootNode == parentNode)
        {
            BPlusTreeNode *newRoot = new BPlusTreeNode(maxKeys);
            newRoot->numKeys = 1;
            newRoot->isLeaf = false;
            newRoot->keys[0] = tempKeys[parentNode->numKeys];
            newRoot->pointers[0] = parentNode;
            newRoot->pointers[1] = newParentNode;

            this->rootNode = newRoot;
        }
        else
        {
            restructureTree(tempKeys[parentNode->numKeys], findParent(this->rootNode, parentNode), newParentNode);
        }
    }
}

BPlusTreeNode *BPlusTree::findParent(BPlusTreeNode *current, BPlusTreeNode *child)
{
    BPlusTreeNode *parent = nullptr;
    if (current->isLeaf)
        return nullptr;

    for (int i = 0; i < current->numKeys + 1; i++)
    {
        if (current->pointers[i] == child)
        {
            parent = current;
            return parent;
        }
        else
        {
            parent = findParent((BPlusTreeNode *)current->pointers[i], child);
            if (parent != nullptr)
                return parent;
        }
    }
    return parent;
}