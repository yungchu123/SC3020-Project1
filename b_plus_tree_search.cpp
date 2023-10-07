#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
#include "types.h"
#include "memory_pool.h"
#include "linked_list_node.h"

#include <vector>
#include <cstring>
#include <iostream>
#include <numeric>

using namespace std;

std::vector<float>  BPlusTree::search(float lowerBoundKey, float upperBoundKey)
{
    
    bool found = false;
    
    // An Array that stores the FG3_PCT_home values returned from the addresses, so that we can calculate based on the values.
    std::vector<float> values;
    
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

       // Display the content of the node.
       displayNode(current_node);

        // std::cout << "Index node accessed. Content is -------";

        while (current_node->isLeaf == false)
        {
            // Iterate through each key in the current node and check which is the key of the lowest value that is bigger than the lower bound.
            for (int i = 0; i < current_node -> numKeys; i++)
            {
                if (lowerBoundKey < current_node -> keys[i])
                {
                    // Get the address of the node that the pointer is pointing towards
                    current_node = (BPlusTreeNode *) current_node -> pointers[i];
                    
                    // Display the content of the node.
                    displayNode(current_node);
                    
                    break;
                }

                if (i == current_node -> numKeys - 1)
                {
                    // go to the node that the right pointer is pointing at to continue searching
                    current_node = (BPlusTreeNode *) current_node -> pointers[i + 1];
            
                    // Display the content of the node.
                    displayNode(current_node);
                    
                    break;
                }

            }

            // Here, the current node is a leaf node.

            // We should keep searching until we reach a key that is more than the upperBoundKey.
            bool exploredRange = false;

            Address* addressOfwithinBound;

            // while we have not found the key that is higher than the upperBoundKey
            while (exploredRange == false)
            {
                // iterate through all the keys in the current node
                for (int i = 0; i < current_node -> numKeys; i++)
                {
                    // If the key is within the lowerBoundKey and upperBoundKey
                    if (current_node -> keys[i] >= lowerBoundKey && current_node -> keys[i] <= upperBoundKey)
                    {
                        // If it is stored in a linked list, get the pointer to the linked list.
                        Address* addressOfLLwithinBound = (Address*)current_node -> pointers[i];
                        LL linkedList = *(LL *)disk->loadFromDisk(*addressOfLLwithinBound, sizeof(LL));

                        // Get all the values in the linked list.
                        for (int i = 0; i < linkedList.getNumRecords(); i++)
                        {
                            // Enter the key into the values array for the number of records with the key.
                            values.push_back(linkedList.getKey());
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
                    if (i == current_node -> numKeys && current_node -> keys[i] < upperBoundKey)
                    {
                        // If there are no other leaf nodes left to explore
                        if (current_node->pointers[maxKeys] == nullptr)
                        {
                            // We can conclude that we have explored the range.
                            exploredRange = true;
                        }

                        else
                        {
                            // update the current_node as the next leaf node.
                            current_node = (BPlusTreeNode *)(current_node->pointers[maxKeys]);
                            
                            // Display the content of the node.
                            displayNode(current_node);
                        }
                    }
                }
            }

            if (values.empty())
            {
                std::cout << "No keys in this range" << std::endl;
                return values;
            }

            else
            {
                
                int sum = std::accumulate(values.begin(), values.end(), 0);
                float average = static_cast<float>(sum) / values.size();
                std::cout << "Average of keys is" << average << std::endl;
                return values;
            }
        }
    }
}