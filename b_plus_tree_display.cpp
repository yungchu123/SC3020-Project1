#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
#include "types.h"

#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

void* address;

// Display a node

void BPlusTree::displayNode(BPlusTreeNode* node)
{
    // Print out all contents in the Node as as |Pointer|Key|Pointer
    std::cout << "|";

    for (int i = 0; i < node->numOfKeys; i++)
    {
        address = node->pointers[i];
        std::cout << address << "|";
        std::cout << node -> keys[i] << "|";
    }

    // Print pointer after the last key (if it is an internal node)
    if (node->pointers[node->numOfKeys] != nullptr) 
    {
        std::cout << node->pointers[node->numOfKeys]<< "|";
    }

    // else if it is a leaf node, it won't have a pointer after the last key.
    else 
    {
        std::cout << " Null |";
    }

    std::cout << " x |"; // Print out the node -> keys[node->numOfKeys]


    // if the numOfKeys is lesser than maxKeys, it will have empty keys and pointers
    for (int i = (node -> numOfKeys)+1; i < maxKeys; i++)
    {
        std::cout << " x |";      // Remaining empty keys
        
        if (i == maxKeys - 1)
        {
            // if it is a leaf node, the last pointer might have an address.
            if (node-> pointers[i]!= nullptr)
            {
                std::cout << node -> pointers[i]<< "|";
            }

            else
            {
                std::cout << "  Null  |"; // Remaining empty pointers
            }
        }
    }

    std::cout << endl;
}


// Display a block and its contents in the disk. Assume it's already loaded in main memory.
void BPlusTree::displayTree(BPlusTreeNode* node, int level)
{

    // If tree exists, display all nodes.
    if (node != nullptr)
    {
        for (int i = 0; i < levels; i++)
        {
            std::cout << "   ";
        }
        std::cout << "level" << level << ":";

        displayNode(node);

        if (node -> isLeaf != true)
        {
            for (int i = 0; i < (node->numOfKeys) + 1; i++)
            {
                node = (BPlusTreeNode* )node->pointers[i];
                displayTree(node, level + 1);
            }
        }
    }

    else
    {
        std::cout << "B Plus Tree does not exist" ;
    }
}



