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

std::vector<Address>  BPlusTree::search(float lowerBoundKey, float upperBoundKey)
{
    
    bool found = false;
    
    // An Array that stores the Addresses of the searched records, so that we can calculate values based on them.
    std::vector<Address> listOfAddresses;
    
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
       std::cout << "Index Node accessed, Content is ----" << endl;
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
                    std::cout << "Index Node accessed, Content is ----" << endl;;
                    displayNode(current_node);
                    
                    break;
                }

                if (i == current_node -> numKeys - 1)
                {
                    // go to the node that the right pointer is pointing at to continue searching
                    current_node = (BPlusTreeNode *) current_node -> pointers[i + 1];
            
                    // Display the content of the node.
                    std::cout << "Index Node accessed, Content is ----" << endl;;
                    displayNode(current_node);
                    
                    break;
                }

            }

            // Here, the current node is a leaf node.

            // We should keep searching until we reach a key that is more than the upperBoundKey.
            bool exploredRange = false;

            Address* addressOfLLwithinBound;
            LLNode* traversalNode;
            int j;
            

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
                        addressOfLLwithinBound = (Address*)current_node -> pointers[i];
                        LL linkedList = *(LL *)disk->loadFromDisk(*addressOfLLwithinBound, sizeof(LL));

                        std::cout<< "Entered if statement for the key within range" << endl;

                        // Get all the values in the linked list.
                        traversalNode = linkedList.getHead();
                       
                        j = 0;

                        // while (traversalNode != nullptr)
                        for (int i = 0; i < linkedList.getNumRecords(); i++)
                        {
                            // Enter the key into the listOfAddresses array for the number of records with the key.
                            listOfAddresses.push_back(traversalNode->getRecordAddress());
                            traversalNode = traversalNode -> getNext();
                            std::cout<< "this is " << j << " iteration" << endl;
                            // GameRecord* returnedData = (GameRecord*)disk->loadFromDisk(traversalNode -> getRecordAddress(), sizeof(GameRecord));
                            // returnedData->FG3_PCT_home;
                            // std::cout<< "the key was " << returnedData->FG_PCT_home << endl;
                            // std::cout<< "the value was " << returnedData->FG3_PCT_home << endl;
                            j++;

                        }
                        
                    }

                    // if the key is bigger than the upperBoundKey, we can conclude the search.
                    if (current_node -> keys[i] > upperBoundKey)
                    {
                        exploredRange = true;
                        break;
                    }
                }

                if (exploredRange) break;

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
                    std::cout << "Index Node accessed, Content is ----"<< endl;;
                    displayNode(current_node);
                }
                

            }

            if (listOfAddresses.empty())
            {
                std::cout << "No records in this range" << std::endl;
            }

            std::cout<< "the number of addresses is " << listOfAddresses.size() << endl;
            return listOfAddresses;
        }
    }
}

float BPlusTree::AverageFG3_PCT_home(std::vector<Address> vectorOfAddress, MemoryPool* disk)
{
    // For the length of the vector
    // access the address, get the value and store it in an array

    std::vector<double> listOfFG3_PCT_home;
    int length = vectorOfAddress.size();
    int k = 0;

    for (int i = 0; i < length; i++)
    {
        Address address = vectorOfAddress[i];
        GameRecord* returnedData = (GameRecord*)disk->loadFromDisk(address, sizeof(GameRecord));
        listOfFG3_PCT_home.push_back(returnedData->FG3_PCT_home);
        std::cout<< "this is " << k << " iteration" << endl;
        std::cout<< "pushed data value is " << returnedData->FG3_PCT_home << endl;
        k++;
    }

    // double sum = std::accumulate(listOfFG3_PCT_home.begin(), listOfFG3_PCT_home.end(), 0.0);
    double sum;

    for (int i = 0; i < listOfFG3_PCT_home.size(); i++) {
        sum += listOfFG3_PCT_home[i];
    }

    std::cout<< "sum calculated is " << sum << endl;
    float average = static_cast<float>(sum) / listOfFG3_PCT_home.size();
    std::cout<< "average calculated is " << average << endl;
    std::cout << "Average of FG3_PCT_home for the selected range is " << average << std::endl;
    return average;
}



