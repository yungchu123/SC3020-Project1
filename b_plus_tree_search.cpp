#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
#include "types.h"
#include "memory_pool.h"
#include "linked_list_node.h"

#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

void BPlusTree::search(float lowerBoundKey, float upperBoundKey)
{
    
    bool found = false;
    
    // Tree is empty.
    if (rootOfTree == nullptr)
    {
        throw std::logic_error("Tree is empty.");
    }

    // If Tree is not empty, iterate through root node to search for the right key
    else
    {
        // Store the root of the tree as the current node.
        BPlusTreeNode *current_node = rootOfTree;

       // (TO ADD CODE) Display the content of the index node.

        // std::cout << "Index node accessed. Content is -------";

        while (current_node->isLeaf == false)
        {
            // Iterate through each key in the current node and check which is the key of the lowest value that is bigger than the lower bound.
            for (int i = 0; i < current_node -> numOfKeys; i++)
            {
                if (lowerBoundKey < current_node -> keys[i])
                {
                    // Get the address of the node that the pointer is pointing towards
                    current_node = (BPlusTreeNode *) current_node -> pointers[i];
                    
                    // (TO ADD CODE) Display the content of the index node.
                    break;
                }

                if (i == current_node -> numOfKeys - 1)
                {
                    // go to the node that the right pointer is pointing at to continue searching
                    current_node = (BPlusTreeNode *) current_node -> pointers[i + 1];
            
                    // (TO ADD CODE) Display the content of the index node
                    break;
                }

            }

            // Here, the current node is a leaf node

            // An Array that stores the FG3_PCT_home values returned from the addresses, so that we can calculate based on the values.
            std::vector<float> values;

            // We should keep searching until we reach a key that is more than the upperBoundKey.
            bool exploredRange = false;

            Address* addressOfwithinBound;

            // while we have not found the key that is higher than the upperBoundKey
            while (exploredRange == false)
            {
                // iterate through all the keys in the current node
                for (int i = 0; i < current_node -> numOfKeys; i++)
                {
                    // If the key is within the lowerBoundKey and upperBoundKey
                    if (current_node -> keys[i] >= lowerBoundKey && current_node -> keys[i] <= upperBoundKey)
                    {
                        // If it is stored in a linked list, get the pointer to the linked list.
                        LL* addressOfLLwithinBound = (LL*)current_node -> pointers[i];


                        // Get all the values in the linked list.
                        for (int i = 0; i < addressOfLLwithinBound -> numRecords; i++)
                        {
                            // get the address of the node that the pointer is pointing to.
                            addressOfwithinBound = (addressOfLLwithinBound->head)->recordAddress;

                             // Load the data from the memory address that contains the lower bound key
                            void* dataInAddress = disk->loadFromDisk(*addressOfwithinBound, sizeof(GameRecord));

                            // Go to the address that the address is pointing towards and get the value of FG3_PCT_home
                            values.push_back(((GameRecord* )dataInAddress) ->FG3_PCT_home);

                            //get the next LLNode in the linked list.
                            addressOfLLwithinBound = (addressOfLLwithinBound->head)->next;
                        }
                        
                        // ONLY CONSIDER THIS IF WE ARE NOT STORING IT IN A LINKED LIST.

                        // // get the address of the node that the pointer is pointing to.
                        // addressOfwithinBound = (Address *)current_node -> pointers[i];
                        
                        // // Load the data from the memory address that contains the lower bound key
                        // void* dataInAddress = disk->loadFromDisk(*addressOfwithinBound, sizeof(GameRecord));

                        // // Go to the address that the address is pointing towards and get the value of FG3_PCT_home
                        // values.push_back(((GameRecord* )dataInAddress) ->FG3_PCT_home);
                    }

                    // if the key is bigger than the upperBoundKey, we can conclude the search.
                    if (current_node -> keys[i] > upperBoundKey)
                    {
                        exploredRange = true;
                    }

                    // if the last key in the current node is still lower than the upperBoundKey, we have to explore the next leaf node too.
                    if (i == current_node -> numOfKeys && current_node -> keys[i] < upperBoundKey)
                    {
                        // If there are no other leaf nodes left to explore
                        if (current_node->pointers[maxKeys] == nullptr)
                        {
                            // We can conclude that we have explored the range.
                            exploredRange = true;
                        }

                        // update the current_node as the next leaf node.
                        current_node = (BPlusTreeNode *)(current_node->pointers[maxKeys]);
                        
                    }
                }
            }
                
        }

    }


}