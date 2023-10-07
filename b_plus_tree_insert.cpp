#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
#include "linked_list_node.h"
#include "types.h"
#include <cmath>
#include <iostream>

void BPlusTree::insert(Address address, float key)
{
    std::cout << "Inserting Key = " << key << std::endl;
    // B+ Tree is currently empty

    if (this->rootOfTree == nullptr)
    {
        // Creating a new Linked List for each new key
        LLNode *newLinkedListNode = new LLNode(address);
        LL *newLinkedList = new LL(newLinkedListNode, key);
        // Storing the header of the linked list into Disk
        Address *LLAddress = new Address(disk->saveToDisk((void *)newLinkedList, sizeof(LL)));

        BPlusTreeNode *newRoot = new BPlusTreeNode(maxKeys);
        newRoot->isLeaf = true;
        newRoot->numKeys = 1;
        newRoot->keys[0] = key;
        newRoot->pointers[0] = LLAddress;


        this->rootOfTree = newRoot; // Initialising B+ Tree root node
        this->numNodes = 1;
        this->levels = 1;
        // index->saveToDisk(newRoot, nodeSize);
    }
    // Traverse B+ Tree to find location to insert key
    else
    {

        BPlusTreeNode *current = rootOfTree;
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
            BPlusTreeNode *nextNodeAddress = (BPlusTreeNode *)current->pointers[current->numKeys];
            int i = 0;
            while (key > current->keys[i] && i < current->numKeys)
            {
                i++;
            }
            if (current->keys[i] == key)
            { // Check if duplicate key. If so, a linked list would have been created
                Address *LLAddress = (Address *)current->pointers[i];
                LL linkedList = *(LL *)disk->loadFromDisk(*LLAddress, sizeof(LL));
                linkedList.insert(address);
            }
            else
            {
                for (int j = current->numKeys; j > i; j--)
                {
                    current->keys[j] = current->keys[j - 1];
                    current->pointers[j] = current->pointers[j - 1];
                }

                // Creating a new Linked List for each new key
                LLNode *newLinkedListNode = new LLNode(address);
                LL *newLinkedList = new LL(newLinkedListNode, key);
                // Storing the header of the linked list into Disk
                Address *LLAddress = new Address(disk->saveToDisk((void *)newLinkedList, sizeof(LL)));

                current->keys[i] = key;
                current->pointers[i] = LLAddress;

                current->numKeys++;
                current->pointers[current->numKeys] = nextNodeAddress;
            }
        }
        // Leaf node is filled
        else
        {
            // Arranging the node in ascending order, after inserting new key
            int tempNode[maxKeys + 1];

            for (int i = 0; i < maxKeys; i++)
            {
                if (current->keys[i] == key)
                { // Check if duplicate key. If so, a linked list would have been created
                    Address *LLAddress = (Address *)current->pointers[i];
                    LL linkedList = *(LL *)disk->loadFromDisk(*LLAddress, sizeof(LL));
                    linkedList.insert(address);
                    return;
                }
                tempNode[i] = current->keys[i];
            }

            // Creating a new Linked List for each new key
            LLNode *newLinkedListNode = new LLNode(address);
            LL *newLinkedList = new LL(newLinkedListNode, key);
            // Storing the header of the linked list into Disk
            Address *LLAddress = new Address(disk->saveToDisk((void *)newLinkedList, sizeof(LL)));

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

            this->numNodes++;

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
                        current->pointers[currentIndex] = LLAddress;
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
                        newNode->pointers[newIndex] = LLAddress;
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

            if (this->rootOfTree == current)
            {
                BPlusTreeNode *newRoot = new BPlusTreeNode(maxKeys);
                newRoot->numKeys = 1;
                newRoot->isLeaf = false;
                newRoot->keys[0] = newNode->keys[0];
                newRoot->pointers[0] = current;
                newRoot->pointers[1] = newNode;

                this->rootOfTree = newRoot;
                this->numNodes++;
                this->levels++;
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

        this->numNodes++;

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
        if (this->rootOfTree == parentNode)
        {
            BPlusTreeNode *newRoot = new BPlusTreeNode(maxKeys);
            newRoot->numKeys = 1;
            newRoot->isLeaf = false;
            newRoot->keys[0] = tempKeys[parentNode->numKeys];
            newRoot->pointers[0] = parentNode;
            newRoot->pointers[1] = newParentNode;

            this->rootOfTree = newRoot;
            this->numNodes ++;
            this->levels ++;
        }
        else
        {
            restructureTree(tempKeys[parentNode->numKeys], findParent(this->rootOfTree, parentNode), newParentNode);
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
