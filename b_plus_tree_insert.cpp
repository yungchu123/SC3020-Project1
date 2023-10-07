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
                return;
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
            }
        }
        // Leaf node is filled
        else
        {
            // Arranging the node in ascending order, after inserting new key
            float tempKeys[maxKeys + 1];
            void* tempPointers[maxKeys + 1];

            for (int i = 0; i < maxKeys; i++)
            {
                if (current->keys[i] == key)
                { // Check if duplicate key. If so, a linked list would have been created
                    Address *LLAddress = (Address *)current->pointers[i];
                    LL linkedList = *(LL *)disk->loadFromDisk(*LLAddress, sizeof(LL));
                    linkedList.insert(address);
                    return;
                }
                tempKeys[i] = current->keys[i];
                tempPointers[i] = current->pointers[i];
            }

            // Creating a new Linked List for each new key
            LLNode *newLinkedListNode = new LLNode(address);
            LL *newLinkedList = new LL(newLinkedListNode, key);
            // Storing the header of the linked list into Disk
            Address *LLAddress = new Address(disk->saveToDisk((void *)newLinkedList, sizeof(LL)));

            int newKeyIndex = 0;

            while (key > tempKeys[newKeyIndex] && newKeyIndex < maxKeys)
            {
                newKeyIndex++;
            }

            for (int j = maxKeys; j > newKeyIndex; j--)
            {
                tempKeys[j] = tempKeys[j - 1];
                tempPointers[j] = tempPointers[j-1];
            }
            tempKeys[newKeyIndex] = key;
            tempPointers[newKeyIndex] = LLAddress;

            // Creating new leaf Node
            BPlusTreeNode *newNode = new BPlusTreeNode(maxKeys);
            newNode->isLeaf = true;

            this->numNodes++;

            // Splitting the numKeys between both currentNode and newNode
            current->numKeys = ceil((maxKeys + 1) / 2);
            newNode->numKeys = floor((maxKeys + 1) / 2);

            // Setting the last pointer (, which points to the next node)
            newNode->pointers[maxKeys] = current->pointers[maxKeys];
            current->pointers[maxKeys] = newNode;

            std::cout<<"CurrentNode"<<std::endl;
            this->displayNode(current);
            std::cout << std::endl;
            std::cout<<"NewNode"<<std::endl;
            this->displayNode(newNode);
            std::cout<<"End"<<std::endl;

            // Rearranging the nodes according to the ascending order
            int i;
            for (i = 0; i < current->numKeys; i++)
            {
                current->keys[i] = tempKeys[i];
                current->pointers[i] = tempPointers[i];
            }

            // Then, the new leaf node. Note we keep track of the i index, since we are using the remaining keys and pointers.
            for (int j = 0; j < newNode->numKeys; i++, j++)
            {
                newNode->keys[j] = tempKeys[i];
                newNode->pointers[j] = tempPointers[i];
            }

            // Update keys/ pointers that have been moved to newNode to NULL
            for (int i = current->numKeys; i < maxKeys; i++)
            {
                current->keys[i] = '\0';
                current->pointers[i] = nullptr;
            }

            std::cout<<"CurrentNode"<<std::endl;
            this->displayNode(current);
            std::cout << std::endl;
            std::cout<<"NewNode"<<std::endl;
            this->displayNode(newNode);
            std::cout<<"End"<<std::endl;

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

void BPlusTree::restructureTree(float key, BPlusTreeNode *parentNode, BPlusTreeNode *childNode)
{

    if (parentNode->numKeys < maxKeys)
    { // Parent node has empty space to accomodate a new childNode
        int i = 0;
        // Getting the position to insert the new childNode
        while (key > parentNode->keys[i] && i < parentNode->numKeys)
        {
            i++;
        }
        for (int j = parentNode->numKeys; j > i; j--)
        {
            parentNode->keys[j] = parentNode->keys[j-1];
        }
        for (int j = parentNode->numKeys+1; j > i + 1; j--)
        {
            parentNode->pointers[j] = parentNode->pointers[j-1];
        }

        parentNode->keys[i] = key;
        parentNode->pointers[i + 1] = childNode;
        parentNode->numKeys++;

    }
    else
    { // Parent Node is full --> Requires shifting of tree
        float tempKeys[maxKeys + 1];
        void *tempPointers[maxKeys + 2];

        // Inserting values into tempKeys
        for (int i = 0; i < maxKeys; i++)
        {
            tempKeys[i] = parentNode->keys[i];
        }

        // Inserting values into tempPointers
        for (int i = 0; i < maxKeys + 1; i++)
        {
            tempPointers[i] = parentNode->pointers[i];
        }

        // Find index to insert key
        int i = 0;
        while (key > tempKeys[i] && i < maxKeys)
        {
            i++;
        }

        // Swap all elements higher than index backwards to fit new key.
        int j;
        for (int j = maxKeys; j > i; j--)
        {
            tempKeys[j] = tempKeys[j - 1];
        }
        tempKeys[i] = key;

        // Move all pointers back to fit new child's pointer as well.
        for (int j = maxKeys + 1; j > i + 1; j--)
        {
            tempPointers[j] = tempPointers[j - 1];
        }
        tempPointers[i+1] = childNode;

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
                parentNode->keys[i] = '\0';
            }
        }
        for (int i = parentNode->numKeys + 1; i < maxKeys + 1; i++)
        {
            parentNode->pointers[i] = nullptr;
            
        }
        parentNode->pointers[parentNode->numKeys] = childNode;

        std::cout<<"ParentNode"<<std::endl;
        this->displayNode(parentNode);
        std::cout << std::endl;
        std::cout<<"NewParentNode"<<std::endl;
        this->displayNode(newParentNode);
        std::cout<<"End"<<std::endl;

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
