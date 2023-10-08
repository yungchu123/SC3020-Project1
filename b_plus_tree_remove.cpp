#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
#include "memory_pool.h"
#include "linked_list_node.h"
#include "types.h"
#include <vector>
#include <cstring>
#include <iostream>
using namespace std;
// while tree is not empty and keys in range exist

void BPlusTree::removeRange(float minValue, float maxValue)
{
    if (rootOfTree == nullptr)
    {
        //throw std::logic_error('Tree is empty');
    }
    else
    {
        bool removed = true; // keeping track if key was removed in last iteration
        while (removed)
        {
            removed = false; // reset flag at start of each iteration
            //while we haven't hit a leaf node, if tree is not empty navigate to correct key
            BPlusTreeNode *current_node = rootOfTree; 
            while (current_node->isLeaf == false)
            {
                for (int i = 0; i < current_node->numKeys; i++)
                {
                    if (minValue < current_node->keys[i])
                    {
                        current_node = (BPlusTreeNode *) current_node->pointers[i]; 
                        break;
                    }
                    
                    if (i == current_node->numKeys-1)
                    {
                        current_node = (BPlusTreeNode *) current_node->pointers[i+1];
                        break;
                    }
                    
                }
                
            }

            // at leaf node now 
                int i; 
                for (i = 0; i < current_node->numKeys; i++)
                {
                    // if no key is within range then break
                    if (current_node->keys[i] > maxValue)
                    {
                        break;
                    }
                    
                    if (current_node->keys[i] >= minValue && current_node->keys[i] <= maxValue)
                    {
                        remove(current_node->keys[i]);
                        removed = true; 
                        break;
                    }
                }
            
        
        }
         
    }   

}

void BPlusTree::remove (float key)
{
    if(rootOfTree == nullptr)
    {
        throw std::logic_error("Tree is empty");
    }
    else 
    {
        BPlusTreeNode *leftSibling; 
        BPlusTreeNode *rightSibling; 
        BPlusTreeNode *current_node = rootOfTree; // init current node as root 
        BPlusTreeNode *parentNode; // keeping track of parentNode as we dive deeper
        int leftSiblingIndex, rightSiblingIndex;         // Index of left and right child to borrow from.

        // while not leaf following nodes to correct key 
        while (current_node->isLeaf == false)
        {
            // set parentNode of current node (jic we need to assign new child) and disk address
            parentNode = current_node;

            // set parent of node
            for (int i = 0; i < current_node-> numKeys; i++) // checking through all keys at current node
            {
                leftSiblingIndex = i - 1;
                rightSiblingIndex = i + 1;

                // if key is lesser than current key go to left pointer's node
                if (key < current_node->keys[i])
                {
                    // load node in from disk to main mem
                    // Node *mainMemoryNode = (Node *)index->loadFromDisk(cursor->pointers[i], nodeSize);
                    current_node = (BPlusTreeNode *) current_node->pointers[i];
                    // Move to new node in main memory.
                    //cursor = (Node *)mainMemoryNode
                    break;
                }
                if (i==current_node->numKeys-1)
                {
                    leftSiblingIndex = i; 
                    rightSiblingIndex = i+2;

                    // Load node in from disk to main memory.
                    //Node *mainMemoryNode = (Node *)index->loadFromDisk(cursor->pointers[i + 1], nodeSize);
                    current_node = (BPlusTreeNode *) current_node->pointers[i+1];

                    //move to new node in main memory
                    //cursor = (Node *)mainMemoryNode;
                    break;
                }
                
            }
        }
        bool found = false; // stop finding once we find one of the key that fits range
        int pos; 
        for (pos = 0; pos <current_node->numKeys; pos++) // iterating keys within current node
        {
          if (current_node->keys[pos] == key)
          {
            found = true; 

            break; 
          }
        }
        
        // if key can't be found, return error 
        if (!found)
        { 
          std::cout << "No longer anymore keys to delete" << endl; 
          
          // update numNodes and numNodes deleted
          return;
        }

        // delete linked list 
        // delete linked list code 

        //delete duplicates within the key first
        Address *LLAddress = (Address *)current_node->pointers[pos];
          // works for duplicates to delete too 
          LL linkedlist = *(LL *)disk->loadFromDisk(*LLAddress, sizeof(LL));
          // delete linked list
          linkedlist.LLdelete(); 
        
        //delete key
        for (int i = pos; i < current_node->numKeys; i++)
        {
          // move pointers and keys forward 
          current_node->keys[i] = current_node->keys[i+1]; 
          current_node->pointers[i] = current_node->pointers[i+1]; 
        }

        current_node->numKeys--; // decrement no of keys in current node 

        // move last pointer forward if present 
        current_node->pointers[current_node->numKeys] = current_node->pointers[current_node->numKeys + 1];

        //set all forward pointers from numkey onwards to nullptr
        for (int i = current_node->numKeys+1 ; i < maxKeys + 1; i++)
        {
            current_node->pointers[i] = nullptr; 
        }

        // if current node is root, check for keys 
        if (current_node == rootOfTree)
        {
          if (current_node->numKeys == 0) //if no keys in current node then delete root node

          {
            // deallocate block used to store root node?

            rootOfTree = nullptr; 
          }
          
          std::cout << "Root node has been deleted" << endl;

          return; 
        }
        
        // if deletion not from root, check for underflow 
        if (current_node->numKeys >= (maxKeys + 1) / 2)
        {
          // min no. of keys maintained, no need to rebalance
          std::cout << "Deleted key successfully no underflow" << endl; 
          
          return; 
        }

        //underflow detected 
        //1. try to borrow from left sibling first

        if (leftSiblingIndex >= 0)
        {
          std::cout << "Trying to borrow from left sib" << endl;
          // find left sibling 
          leftSibling = (BPlusTreeNode *)parentNode->pointers[leftSiblingIndex]; 
          // check if we can borrow without underflow 
          if (leftSibling->numKeys >= (maxKeys+1) / 2 + 1)
          {
            // insert borrowed key into leftmost of current node 
            //shift last pointer by 1
            current_node->pointers[current_node->numKeys+1] = current_node->pointers[current_node->numKeys];
            //shift all remaining pointers back by 1
            for (int i = current_node->numKeys; i > 0; i--)
            {
              current_node->keys[i] = current_node->keys[i-1];
              current_node->pointers[i] = current_node->pointers[i-1];

            }

            //transfer borrowed key and pointer 
            current_node->keys[0] = leftSibling->keys[leftSibling->numKeys - 1]; // rightmost key of left sib replaces leftmost key of current node
            current_node->pointers[0] = leftSibling->pointers[leftSibling->numKeys - 1];
            current_node->numKeys++;
            leftSibling->numKeys--; 

            //update left sib 
            leftSibling->pointers[current_node->numKeys] = leftSibling->pointers[current_node->numKeys];

            // update parentNode  key 
            parentNode->keys[leftSiblingIndex] = current_node->keys[0];

            // save parentNode to disk? (add code here)
            // save left sib to disk (add code here)
            // save curr node to disk (add code here)

            return; 
           
          }
        }
        // 2. try to borrow from right sibling 
        if (rightSiblingIndex <= parentNode->numKeys)
        {
          std::cout << "Trying to borrow from right sib" << endl;
          rightSibling = (BPlusTreeNode *) parentNode->pointers[rightSiblingIndex]; 

          // check if we can borrow without underflow 
          if (rightSibling->numKeys >= (maxKeys + 1) / 2+1 )
          {
            //shift last pointer of current node by one 
            current_node->pointers[current_node->numKeys+1]=current_node->pointers[current_node->numKeys];
            
            //transfer borrowed key and pointer 
            current_node->keys[current_node->numKeys] = rightSibling->keys[0];
            current_node->pointers[current_node->numKeys] = rightSibling->pointers[0];
            current_node->numKeys++;
            rightSibling->numKeys--;

            //update right sib, shift keys and pointers left 
            for (int i = 0; i < rightSibling->numKeys; i++)
            {
              rightSibling->keys[i] = rightSibling->keys[i+1];
              rightSibling->pointers[i] = rightSibling->pointers[i+1];

            }

            // move right sib last pointer left by 1
            rightSibling->pointers[current_node->numKeys] = rightSibling->pointers[current_node->numKeys + 1];

            // update parentNode node key to be new lower bound of right sib 
            parentNode->keys[rightSiblingIndex-1] = rightSibling->keys[0];

            //save parentNode to disk (add code here)
            //save right sib to disk (add code here)
            //save current node to disk (add code here)

            return; 
            
            }
            
          }
          //3. merge with left sibling  
          if (leftSiblingIndex >= 0)
          {
            std::cout << "Trying to merge with left sib" << endl;
            leftSibling = (BPlusTreeNode *) parentNode->pointers[leftSiblingIndex];

            // transfer all keys and pointers from cur node to left node 
            for (int i = leftSibling->numKeys, j = 0; j < current_node->numKeys; i++, j++)
            {
              leftSibling->keys[i] = current_node->keys[j];
              leftSibling->pointers[i] = current_node->pointers[j];
            }
            
            // update keys and pointer. make left sib last pointer point to next leaf node pointed to by current
            leftSibling->numKeys += current_node->numKeys;
            leftSibling->pointers[leftSibling->numKeys] = current_node->pointers[current_node->numKeys];

            // save left sib to disk (add code here)

            // remove internal to update parentNode
            //(add code here)
            removeInternal(parentNode->keys[leftSiblingIndex], (BPlusTreeNode *) parentNode, (BPlusTreeNode *) current_node);

            // aft updating parentNode delete current node from disk
            return; 
          }
          //4. merge with right sib
          else if (rightSiblingIndex <= parentNode->numKeys)
          {
            std::cout << "Trying to merge with right sib" << endl;
            rightSibling = (BPlusTreeNode *) parentNode->pointers[rightSiblingIndex]; 

            // moving right sib's keys into ours 
            // transfer all keys and pointers from right sib into current
            for (int i = current_node->numKeys, j=0; j<rightSibling->numKeys; i++, j++)
            {
              current_node->keys[i] = rightSibling->keys[j];
              current_node->pointers[i] = rightSibling->pointers[j];
            }
            
            // update variables and make cur node's last pointer point to next leaf node pointed to by right sib
            current_node->numKeys += rightSibling->numKeys;
            current_node->pointers[current_node->numKeys] = rightSibling->pointers[rightSibling->numKeys];

            // save current node to disk (add code here)
            // remove internal to update parentNode and fully remove right node (add code here)
            removeInternal(parentNode->keys[rightSiblingIndex-1], (BPlusTreeNode *)parentNode, (BPlusTreeNode *)rightSibling);
            // delete right node from disk 
            return; 
          }
    }
}


// remove internal nodes

void BPlusTree::removeInternal(float key, BPlusTreeNode *parentNode, BPlusTreeNode *childNode)

{
  //load in current node -- parent, and child 
  
  // if current parent is root 
  if (parentNode == rootOfTree)
  {
    //and if we have to remove all keys in root, 
    //then we need to change the root to its child 
    if (parentNode->numKeys == 1) 
    {
      // if larger pointer points to child, make it new root?
      if (parentNode->pointers[1] == childNode)
      {
        //delete child 
        childNode = nullptr; 
        //set new root to be parent's left pointer
        rootOfTree = (BPlusTreeNode *) parentNode->pointers[0];

        // delete old root -- parent 
        parentNode = nullptr; 

        std::cout << "root node has changed" << endl;
        return;
      }
      // else if left pointer in root -- parent, contains the child then delete from there
      else if (parentNode->pointers[0] == childNode)
      {
        childNode = nullptr; //delete childnode
        // set new root as parent's right pointer
        rootOfTree = (BPlusTreeNode *) parentNode->pointers[1];
        //delete old root -- parent
        parentNode = nullptr; 
        std::cout << "root node has changed" << endl; 
        return;
      }
      
    }
    
  }
  
  // parent is not the root 
  // may need to delete internal node recursively
  int pos; // position of key in parent node

  //searching for key to delete in parent node based on child's lower bound key
  for (pos = 0; pos < parentNode->numKeys; pos++)
  {
    if (parentNode->keys[pos] == key)
    {
      break;
    }
    
  }
  
  // delete key by shifting all keys forward
  for (int i = pos; i < parentNode->numKeys; i++)
  {
    parentNode->keys[i] = parentNode->keys[i+1];
  }
  
  //search for pointer to delete in parent
  for (pos = 0; pos < parentNode->numKeys; pos++)
  {
    if (parentNode->pointers[pos] == childNode)
    {
      break;
    }
    
  }
  
  // moving all pointers from that point forward by one to delete
  for (int i = pos; i < parentNode->numKeys + 1; i++)
  {
    parentNode->pointers[i] = parentNode->pointers[i+1];
  }
  
  // update no. of keys
  parentNode->numKeys--;

  //check for underflow in parent 
  if (parentNode->numKeys >= (maxKeys + 1) / 2 - 1)
  {
    return; 
  }
  
  //underflow in parent's keys
  //try to borrow from siblings
  if (parentNode == rootOfTree)
  {
    return;
  }

  // need to find the parent of this parent to get our siblings
  BPlusTreeNode *leftSibling, *rightSibling;
  int leftSiblingIndex, rightSiblingIndex;
  BPlusTreeNode *newParent = findParent(parentNode, childNode);

  // find left and right sibling of old parent
  for (int pos = 0; pos < newParent->numKeys + 1; pos++) // not too sure about this part
  {
    if (newParent->pointers[pos] == parentNode)
    {
      leftSiblingIndex = pos - 1;
      rightSiblingIndex = pos + 1;
      break;
    }
  }
  
  //1. try to borrow from left sibling
  if (leftSiblingIndex >= 0)
  {
    leftSibling = (BPlusTreeNode *) newParent->pointers[leftSiblingIndex];
    // check if can borrow 
    // non leaf require mininum of floor n/2
    if (leftSibling->numKeys >= (maxKeys + 1) / 2)
    {
      //insert borrowed key into leftmost of current node -- parent
      // shift all remaining pointers by one

      for (int i = parentNode->numKeys; i > 0; i--)
      {
        parentNode->keys[i] = parentNode->keys[i-1];
      }
      
      //transfer borrwed key and pointer to currentnode from left node
      parentNode->keys[0] = newParent->keys[leftSiblingIndex];
      newParent->keys[leftSiblingIndex] = leftSibling->keys[leftSibling->numKeys -1];

      // move all pointers back to fit a new one 
      for (int i = parentNode->numKeys + 1; i > 0; i--)
      {
        parentNode->pointers[i] = parentNode->pointers[i-1];
      }
      
      // add pointers to current node from left node
      parentNode->pointers[0] = leftSibling->pointers[leftSibling->numKeys];

      //change key numbers
      parentNode->numKeys++; 
      leftSibling->numKeys--;

      //update left sibling, by shifting pointers left
      leftSibling->pointers[parentNode->numKeys] = leftSibling->pointers[parentNode->numKeys + 1];

      //save parent to disk 
      // save left sibling to disk 
      // save current node to disk
      return;
    }
    
  }
  
  //2. borrow from right sib
  //check for right sib
  if (rightSiblingIndex <= newParent->numKeys)
  {
    rightSibling = (BPlusTreeNode *) newParent->pointers[rightSiblingIndex]; 

    // check if we can borrow without underflow 
    if (rightSibling->numKeys >= (maxKeys + 1) / 2)
    {
      // no need to shift remaining pointers since we are inserting on the right
      // transfer borrowed key and pointer (left of right node) to right of current node -- parent
      parentNode->keys[parentNode->numKeys] = newParent->keys[pos];
      parentNode->keys[pos] = rightSibling->keys[0];

      //update right sib (shifting keys and pointers left)
      for (int i = 0; i < rightSibling->numKeys - 1; i++)
      {
        rightSibling->keys[i] = rightSibling->keys[i+1];
      }
      
      // transfer first pointer from right sibling to current node
      parentNode->pointers[parentNode->numKeys + 1] = rightSibling->pointers[0];

      //shift pointers left for right node to delete first pointer
      for (int i = 0; i < rightSibling->numKeys; ++i)
      {
        rightSibling->pointers[i] = rightSibling->pointers[i+1];
      }
      
      //update no. of keys
      parentNode->numKeys++;
      rightSibling->numKeys--; 

      //save parent to disk
      //save right sib to disk
      //save current node to disk
      return;
    }
    
    
  }
  //3. can't borrow so merge with left sib
  if (leftSiblingIndex >= 0)
  {
    leftSibling = (BPlusTreeNode *) newParent->pointers[leftSiblingIndex];

    // make left sib upper bound to be current node's lower bound
    leftSibling->keys[leftSibling->numKeys] = newParent->keys[leftSiblingIndex];

    //transfer all keys from current node to left node
    for (int i = leftSibling->numKeys + 1, j = 0; j < parentNode->numKeys; j++)
    {
      leftSibling->keys[i] = parentNode->keys[j];
    }
    
    //transfer all pointers
    for (int i = leftSibling->numKeys + 1, j = 0; j < parentNode->numKeys + 1; j++)
    {
      leftSibling->pointers[i] = parentNode->pointers[j];
      parentNode->pointers[j] = nullptr; 
    }
    
    // update variables, make left sib last pointer point to the next leaf node that was pointed by the cur node
    leftSibling->numKeys += parentNode->numKeys+1; 
    parentNode->numKeys = 0;

    //save left node to disk 
    //delete current node
    removeInternal(newParent->keys[leftSiblingIndex], newParent, parentNode);
  }
  //4. merge with right sib
  else if (rightSiblingIndex <= newParent->numKeys)
  {
    rightSibling = (BPlusTreeNode *) newParent->pointers[rightSiblingIndex];

    // set upper bound of  current node to be lower bound of right sib
    parentNode->keys[parentNode->numKeys] = newParent->keys[rightSiblingIndex - 1];

    // moving right node's contents into current node
    // transfer all keys from right sib into current
    for (int i = parentNode->numKeys + 1, j = 0; j < rightSibling->numKeys; j++)
    {
      parentNode->keys[i] = rightSibling->keys[j];
    }
    
    // transfer all pointers from right sib into current
    for (int i = parentNode->numKeys+1, j = 0; j < rightSibling->numKeys + 1; j++)

    {
      parentNode->pointers[i] = rightSibling->pointers[j];
      rightSibling->pointers[j] = nullptr;
    }

    //update variables 
    parentNode->numKeys += rightSibling->numKeys + 1;
    rightSibling->numKeys = 0; 

    //save current node to disk 
    //delete right node
    removeInternal(newParent->keys[rightSiblingIndex-1], newParent, rightSibling);
    
  }
  
}




// need to bear in mind that other leaf nodes could have keys that fall within the range
// found leaf nodes with gameRecord.FG_PCT_home <= threshold
// for each leaf node
// iterate through entries and if gameRecord.FG_PCT_home <= threshold then delete entry
// delete key. if range no longer yields results, then deletion complete
// AT ALL TIMES make sure keys are SORTED and tree is BALANCED
// handle underflow: check if node satisfies minimum No of keys
// if not then 1) borrow from sibling node and update parentNode if necessary
// else 2) merge with sibling node and update parentNode if necessary
// repeat this underflow handling process for parentNode nodes recursively until I reach root node