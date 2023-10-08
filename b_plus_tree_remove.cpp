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
                        std::cout << "can't find key, leaving removeRange" << endl; 
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
          std::cout << "Unable to find" << key << "to delete" << endl; 
          
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
          std::cout << "Deleted key" << key << "successfully no underflow" << endl; 
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

  // Check if there's underflow in parent
  // No underflow, life is good.
  if (cursor->numOfKeys >= (maxKeys + 1) / 2 - 1)
  {
    return;
  }

  // If we reach here, means there's underflow in parent's keys.
  // Try to steal some from neighbouring nodes.
  // If we are the root, we are screwed. Just give up.
  if (cursorDiskAddress == addressOfRootNode)
  {
    return;
  }

  // If not, we need to find the parent of this parent to get our siblings.
  // Pass in lower bound key of our child to search for it.
  //Node *parentDiskAddress = findParent((Node *)addressOfRootNode, cursorDiskAddress, cursor->keys[0]);
  int leftSibling, rightSibling;

  // Load parent into main memory.
  Address parentAddress{parentDiskAddress, 0};
  Node *parent = (Node *)index->loadFromDisk(parentAddress, sizeOfNode);

  // Find left and right sibling of cursor, iterate through pointers.
  for (pos = 0; pos < parent->numOfKeys + 1; pos++)
  {
    if (parent->pointers[pos].blockAddress == cursorDiskAddress)
    {
      leftSibling = pos - 1;
      rightSibling = pos + 1;
      break;
    }
  }

  // Try to borrow a key from either the left or right sibling.
  // Check if left sibling exists. If so, try to borrow.
  if (leftSibling >= 0)
  {
    // Load in left sibling from disk.
    Node *leftNode = (Node *)index->loadFromDisk(parent->pointers[leftSibling], sizeOfNode);

    // Check if we can steal (ahem, borrow) a key without underflow.
    // Non leaf nodes require a minimum of ⌊n/2⌋
    if (leftNode->numOfKeys >= (maxKeys + 1) / 2)
    {
      // We will insert this borrowed key into the leftmost of current node (smaller).
      // Shift all remaining keys and pointers back by one.
      for (int i = cursor->numOfKeys; i > 0; i--)
      {
        cursor->keys[i] = cursor->keys[i - 1];
      }

      // Transfer borrowed key and pointer to cursor from left node.
      // Basically duplicate cursor lower bound key to keep pointers correct.
      cursor->keys[0] = parent->keys[leftSibling];
      parent->keys[leftSibling] = leftNode->keys[leftNode->numOfKeys - 1];

      // Move all pointers back to fit new one
      for (int i = cursor->numOfKeys + 1; i > 0; i--)
      {
        cursor->pointers[i] = cursor->pointers[i - 1];
      }

      // Add pointers to cursor from left node.
      cursor->pointers[0] = leftNode->pointers[leftNode->numOfKeys];

      // Change key numbers
      cursor->numOfKeys++;
      leftNode->numOfKeys--;

      // Update left sibling (shift pointers left)
      leftNode->pointers[cursor->numOfKeys] = leftNode->pointers[cursor->numOfKeys + 1];

      // Save parent to disk.
      Address parentAddress{parentDiskAddress, 0};
      index->saveToDisk(parent, sizeOfNode, parentAddress);

      // Save left sibling to disk.
      index->saveToDisk(leftNode, sizeOfNode, parent->pointers[leftSibling]);

      // Save current node to disk.
      Address cursorAddress = {cursorDiskAddress, 0};
      index->saveToDisk(cursor, sizeOfNode, cursorAddress);
      return;
    }
  }

  // If we can't take from the left sibling, take from the right.
  // Check if we even have a right sibling.
  if (rightSibling <= parent->numOfKeys)
  {
    // If we do, load in right sibling from disk.
    Node *rightNode = (Node *)index->loadFromDisk(parent->pointers[rightSibling], sizeOfNode);

    // Check if we can steal (ahem, borrow) a key without underflow.
    if (rightNode->numOfKeys >= (maxKeys + 1) / 2)
    {
      // No need to shift remaining pointers and keys since we are inserting on the rightmost.
      // Transfer borrowed key and pointer (leftmost of right node) over to rightmost of current node.
      cursor->keys[cursor->numOfKeys] = parent->keys[pos];
      parent->keys[pos] = rightNode->keys[0];

      // Update right sibling (shift keys and pointers left)
      for (int i = 0; i < rightNode->numOfKeys - 1; i++)
      {
        rightNode->keys[i] = rightNode->keys[i + 1];
      }

      // Transfer first pointer from right node to cursor
      cursor->pointers[cursor->numOfKeys + 1] = rightNode->pointers[0];

      // Shift pointers left for right node as well to delete first pointer
      for (int i = 0; i < rightNode->numOfKeys; ++i)
      {
        rightNode->pointers[i] = rightNode->pointers[i + 1];
      }

      // Update numKeys
      cursor->numOfKeys++;
      rightNode->numOfKeys--;

      // Save parent to disk.
      Address parentAddress{parentDiskAddress, 0};
      index->saveToDisk(parent, sizeOfNode, parentAddress);

      // Save right sibling to disk.
      index->saveToDisk(rightNode, sizeOfNode, parent->pointers[rightSibling]);

      // Save current node to disk.
      Address cursorAddress = {cursorDiskAddress, 0};
      index->saveToDisk(cursor, sizeOfNode, cursorAddress);
      return;
    }
  }

  // If we reach here, means no sibling we can steal from.
  // To resolve underflow, we must merge nodes.

  // If left sibling exists, merge with it.
  if (leftSibling >= 0)
  {
    // Load in left sibling from disk.
    Node *leftNode = (Node *)index->loadFromDisk(parent->pointers[leftSibling], sizeOfNode);

    // Make left node's upper bound to be cursor's lower bound.
    leftNode->keys[leftNode->numOfKeys] = parent->keys[leftSibling];

    // Transfer all keys from current node to left node.
    // Note: Merging will always suceed due to ⌊(n)/2⌋ (left) + ⌊(n-1)/2⌋ (current).
    for (int i = leftNode->numOfKeys + 1, j = 0; j < cursor->numOfKeys; j++)
    {
      leftNode->keys[i] = cursor->keys[j];
    }

    // Transfer all pointers too.
    Address nullAddress{nullptr, 0};
    for (int i = leftNode->numOfKeys + 1, j = 0; j < cursor->numOfKeys + 1; j++)
    {
      leftNode->pointers[i] = cursor->pointers[j];
      cursor->pointers[j] = nullAddress;
    }

    // Update variables, make left node last pointer point to the next leaf node pointed to by current.
    leftNode->numOfKeys += cursor->numOfKeys + 1;
    cursor->numOfKeys = 0;

    // Save left node to disk.
    index->saveToDisk(leftNode, sizeOfNode, parent->pointers[leftSibling]);

    // Delete current node (cursor)
    // We need to update the parent in order to fully remove the current node.
    removeInternal(parent->keys[leftSibling], (Node *)parentDiskAddress, (Node *)cursorDiskAddress);
  }
  // If left sibling doesn't exist, try to merge with right sibling.
  else if (rightSibling <= parent->numOfKeys)
  {
    // Load in right sibling from disk.
    Node *rightNode = (Node *)index->loadFromDisk(parent->pointers[rightSibling], sizeOfNode);

    // Set upper bound of cursor to be lower bound of right sibling.
    cursor->keys[cursor->numOfKeys] = parent->keys[rightSibling - 1];

    // Note we are moving right node's stuff into ours.
    // Transfer all keys from right node into current.
    // Note: Merging will always suceed due to ⌊(n)/2⌋ (left) + ⌊(n-1)/2⌋ (current).
    for (int i = cursor->numOfKeys + 1, j = 0; j < rightNode->numOfKeys; j++)
    {
      cursor->keys[i] = rightNode->keys[j];
    }

    // Transfer all pointers from right node into current.
    Address nullAddress = {nullptr, 0};
    for (int i = cursor->numOfKeys + 1, j = 0; j < rightNode->numOfKeys + 1; j++)
    {
      cursor->pointers[i] = rightNode->pointers[j];
      rightNode->pointers[j] = nullAddress;
    }

    // Update variables
    cursor->numOfKeys += rightNode->numOfKeys + 1;
    rightNode->numOfKeys = 0;

    // Save current node to disk.
    Address cursorAddress{cursorDiskAddress, 0};
    index->saveToDisk(cursor, sizeOfNode, cursorAddress);

    // Delete right node.
    // We need to update the parent in order to fully remove the right node.
    void *rightNodeAddress = parent->pointers[rightSibling].blockAddress;
    removeInternal(parent->keys[rightSibling - 1], (Node *)parentDiskAddress, (Node *)rightNodeAddress);
  }
}
// removeLL function (Add code here)
void BPlusTree::removeLL(Address LLHeadAddress)
{
  // Load in first node from disk.
  Node *head = (Node *)index->loadFromDisk(LLHeadAddress, sizeOfNode);

  // Removing the current head. Simply deallocate the entire block since it is safe to do so for the linked list
  // Keep going down the list until no more nodes to deallocate.

  // Deallocate the current node.
  index->deallocate(LLHeadAddress, sizeOfNode);

  // End of linked list
  if (head->pointers[head->numOfKeys].blockAddress == nullptr)
  {
    std::cout << "End of linked list";
    return;
  }

  if (head->pointers[head->numOfKeys].blockAddress != nullptr)
  {

    removeLL(head->pointers[head->numOfKeys]);
  }
}
// underflow detected
void BPlusTree::borrowOrMerge(Node* cursor, Node* parent, int leftSibling, int rightSibling) {
    // Check if left sibling has more than minimum keys
    if(leftSibling >= 0) 
    {
        // load left sibling from disk
        Node *leftNode = (Node *)index->loadFromDisk(parent->pointers[leftSibling], sizeOfNode);
        // check if we can borrow from left sibling
        // Check if we can steal (ahem, borrow) a key without underflow.
        if (leftNode->numOfKeys >= (maxKeys + 1) / 2 + 1)
        {
            // insert this borrowed key into the leftmost of current node (smaller).

            // Shift last pointer back by one first.
            cursor->pointers[cursor->numOfKeys + 1] = cursor->pointers[cursor->numOfKeys];

            // Shift all remaining keys and pointers back by one.
            for (int i = cursor->numOfKeys; i > 0; i--)
            {
            cursor->keys[i] = cursor->keys[i - 1];
            cursor->pointers[i] = cursor->pointers[i - 1];
            }

            // Transfer borrowed key and pointer (rightmost of left node) over to current node.
            cursor->keys[0] = leftNode->keys[leftNode->numOfKeys - 1];
            cursor->pointers[0] = leftNode->pointers[leftNode->numOfKeys - 1];
            cursor->numOfKeys++;
            leftNode->numOfKeys--;

            // Update left sibling (shift pointers left)
            leftNode->pointers[cursor->numOfKeys] = leftNode->pointers[cursor->numOfKeys + 1];

            // Update parent node's key
            parent->keys[leftSibling] = cursor->keys[0];

            // Save parent to disk.
            Address parentAddress{parentDiskAddress, 0};
            index->saveToDisk(parent, sizeOfNode, parentAddress);

            // Save left sibling to disk.
            index->saveToDisk(leftNode, sizeOfNode, parent->pointers[leftSibling]);

            // Save current node to disk.
            Address cursorAddress = {cursorDiskAddress, 0};
            index->saveToDisk(cursor, sizeOfNode, cursorAddress);
        
            // update numNodes and numNodesDeleted after deletion
            int numNodesDeleted = numNodes - index->getAllocated();
            numNodes = index->getAllocated();
        }
    }
    else if(rightSibling < parent->numOfKeys) 
    {
        //load right sibling from disk
        Node *rightNode = (Node *)index->loadFromDisk(parent->pointers[rightSibling], sizeOfNode);
        // check if we can borrow from right sibling
        if (rightNode->numOfKeys >= (maxKeys + 1) / 2 + 1)
        {

        // We will insert this borrowed key into the rightmost of current node (larger).
        // Shift last pointer back by one first.
        cursor->pointers[cursor->numOfKeys + 1] = cursor->pointers[cursor->numOfKeys];

        // No need to shift remaining pointers and keys since we are inserting on the rightmost.
        // Transfer borrowed key and pointer (leftmost of right node) over to rightmost of current node.
        cursor->keys[cursor->numOfKeys] = rightNode->keys[0];
        cursor->pointers[cursor->numOfKeys] = rightNode->pointers[0];
        cursor->numOfKeys++;
        rightNode->numOfKeys--;

        // Update right sibling (shift keys and pointers left)
        for (int i = 0; i < rightNode->numOfKeys; i++)
        {
          rightNode->keys[i] = rightNode->keys[i + 1];
          rightNode->pointers[i] = rightNode->pointers[i + 1];
        }

        // Move right sibling's last pointer left by one too.
        rightNode->pointers[cursor->numOfKeys] = rightNode->pointers[cursor->numOfKeys + 1];

        // Update parent node's key to be new lower bound of right sibling.
        parent->keys[rightSibling - 1] = rightNode->keys[0];

        // Save parent to disk.
        Address parentAddress{parentDiskAddress, 0};
        index->saveToDisk(parent, sizeOfNode, parentAddress);

        // Save right sibling to disk.
        index->saveToDisk(rightNode, sizeOfNode, parent->pointers[rightSibling]);

        // Save current node to disk.
        Address cursorAddress = {cursorDiskAddress, 0};
        index->saveToDisk(cursor, sizeOfNode, cursorAddress);

        // update numNodes and numNodesDeleted after deletion
        int numNodesDeleted = numNodes - index->getAllocated();
        numNodes = index->getAllocated();      
        }
    }

    // no borrowing possible, merge instead to solve underflow
    else if(leftSibling >= 0) 
    {
        //load left sibling from disk
        Node *leftNode = (Node *)index->loadFromDisk(parent->pointers[leftSibling], sizeOfNode);
        // transfer all keys and pointers from current node to left node.
        // merge with left sibling
        for (int i = leftNode->numOfKeys, j = 0; j < cursor->numOfKeys; i++, j++)
        {
            leftNode->keys[i] = cursor->keys[j];
            leftNode->pointers[i] = cursor->pointers[j];
        }

        // Update variables, make left node last pointer point to the next leaf node pointed to by current.
        leftNode->numOfKeys += cursor->numOfKeys;
        leftNode->pointers[leftNode->numOfKeys] = cursor->pointers[cursor->numOfKeys];

        // Save left node to disk.
        index->saveToDisk(leftNode, sizeOfNode, parent->pointers[leftSibling]);

        // We need to update the parent in order to fully remove the current node.
        removeInternal(parent->keys[leftSibling], (Node *)parentDiskAddress, (Node *)cursorDiskAddress);

        // Now that we have updated parent, we can just delete the current node from disk.
        Address cursorAddress{cursorDiskAddress, 0};
        index->deallocate(cursorAddress, sizeOfNode);

    }
    else if(rightSibling < parent->numOfKeys) 
    {
        // load right sibling from disk.
        Node *rightNode = (Node *)index->loadFromDisk(parent->pointers[rightSibling], sizeOfNode);

        // Moving right node's keys into current node.
        // Transfer all keys and pointers from right node into current.
        // merge with right sib
        for (int i = cursor->numOfKeys, j = 0; j < rightNode->numOfKeys; i++, j++)
        {
            cursor->keys[i] = rightNode->keys[j];
            cursor->pointers[i] = rightNode->pointers[j];
        }

        // Update variables, make current node last pointer point to the next leaf node pointed to by right node.
        cursor->numOfKeys += rightNode->numOfKeys;
        cursor->pointers[cursor->numOfKeys] = rightNode->pointers[rightNode->numOfKeys];

        // Save current node to disk.
        Address cursorAddress{cursorDiskAddress, 0};
        index->saveToDisk(cursor, sizeOfNode, cursorAddress);

        // We need to update the parent in order to fully remove the right node.
        void *rightNodeAddress = parent->pointers[rightSibling].blockAddress;
        removeInternal(parent->keys[rightSibling - 1], (Node *)parentDiskAddress, (Node *)rightNodeAddress);

        // Now that we have updated parent, we can just delete the right node from disk.
        Address rightNodeDiskAddress{rightNodeAddress, 0};
        index->deallocate(rightNodeDiskAddress, sizeOfNode);
    }
}

// need to bear in mind that other leaf nodes could have keys that fall within the range
// found leaf nodes with gameRecord.FG_PCT_home <= threshold
// for each leaf node
// iterate through entries and if gameRecord.FG_PCT_home <= threshold then delete entry
// delete key. if range no longer yields results, then deletion complete
// AT ALL TIMES make sure keys are SORTED and tree is BALANCED
// handle underflow: check if node satisfies minimum No of keys
// if not then 1) borrow from sibling node and update parent if necessary
// else 2) merge with sibling node and update parent if necessary
// repeat this underflow handling process for parent nodes recursively until I reach root node
