#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
#include "linked_list_node.h"
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
    
    for (int i = 0; i < this->maxKeys; i++){
        if (node->pointers[i] != nullptr){
            std::cout << node->pointers[i] << "|";
        } else {
            std::cout << "-" << "|";
        }
        
        std::cout << node->keys[i] << "|";
    }
    if (node->pointers[maxKeys] != nullptr){
            std::cout << node->pointers[maxKeys] << "|";
        } else {
            std::cout << "-" << "|";
        }
    
    /***
    for (int i = 0; i < node->numKeys; i++)
    {
        address = node->pointers[i];
        std::cout << address << "|";
        std::cout << node -> keys[i] << "|";
    }

    // Print pointer after the last key (if it is an internal node)
    if (node->pointers[node->numKeys] != nullptr) 
    {
        std::cout << node->pointers[node->numKeys]<< "|";
    }

    // else if it is a leaf node, it won't have a pointer after the last key.
    else 
    {
        std::cout << " Null |";
    }

    std::cout << " x |"; // Print out the node -> keys[node->numOfKeys]


    // if the numOfKeys is lesser than maxKeys, it will have empty keys and pointers
    for (int i = (node -> numKeys)+1; i < maxKeys; i++)
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
    ***/
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
            for (int i = 0; i < (node->numKeys) + 1; i++)
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

void BPlusTree::displayBlock(Address *address)
{
    void *block = operator new(sizeOfNode);
    std::memcpy(block, address, sizeOfNode);

    unsigned char *blockChar = (unsigned char*)address;

    int i = 0;

    while (i < sizeOfNode)
    {
        void *recordAddress = operator new((sizeof(GameRecord)));
        std::memcpy(recordAddress, blockChar, sizeof(GameRecord));

        GameRecord * gameRecord = (GameRecord *) recordAddress;

        std::cout << "[" << gameRecord->GAME_DATE_EST << gameRecord->TEAM_ID_home << gameRecord->PTS_home << gameRecord->FG_PCT_home << gameRecord->FT_PCT_home << gameRecord->FG3_PCT_home << gameRecord->AST_home << gameRecord->REB_home << gameRecord->HOME_TEAM_WINS;
        blockChar += sizeof(GameRecord);
        i += sizeof(GameRecord);
    }
}



// Displays the LinkedList that stores the records.
void BPlusTree::displayLL(LL* LinkedList)
{
    LLNode* llnode = LinkedList -> getHead();
    
    for (int i = 0; i < LinkedList->getNumRecords(); i++)
    {
        Address address = llnode->getRecordAddress() ;
        std::cout << "\nData block accessed. Content is -----";
        displayBlock(&address);
        std::cout << endl;
        llnode = llnode -> getNext();
    }
}



