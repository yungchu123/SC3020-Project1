#include "b_plus_tree.h"
#include "types.h"

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
        Node *current_node = rootOfTree;

       // (TO ADD CODE) Display the content of the index node.

        // std::cout << "Index node accessed. Content is -------";

        

        while (current_node->getIsLeaf() == false)
        {
            // Iterate through each key in the current node and check which is the key of the lowest value that is bigger than the lower bound.
            for (int i = 0; i < current_node -> getNumOfKeys(); i++)
            {
                if (lowerBoundKey < current_node -> getKeys()[i])
                {
                    // Go to the node that the current node is pointing at.
                    Address nextNode = current_node -> getPointers()[i];
                    current_node = (Node *) nextNode.blockAddress;
                    // (TO ADD CODE) Display the content of the index node.
                    break;
                }

                if (i == current_node -> getNumOfKeys() - 1)
                {
                    // go to the node that the right pointer is pointing at to continue searching
                    Address nextNode = current_node -> getPointers()[i + 1];
                    current_node = (Node*) nextNode.blockAddress;
                    // (TO ADD CODE) Display the content of the index node
                    break;
                }

            }

            // TO REVAMP THE BELOW PART

            for (int i = 0; i < current_node -> getNumOfKeys(); i++)
            {
                if (lowerBoundKey == current_node -> getKeys()[i])
                {
                    bool found = true;
                    // (TO ADD CODE) do the next step, which is to access the record
                    // (TO ADD CODE) Display the content of the index node.
                    break;
                }

                if (i == current_node -> getNumOfKeys() - 1)
                {
                    // (TO ADD CODE) Error message saying that cannot find the record looking for.
                    // (TO ADD CODE) Display the content of the index node
                    break;
                }
            }






            
                
        }

    }


}