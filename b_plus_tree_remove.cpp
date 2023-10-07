#include "b_plus_tree.h"
#include "b_plus_tree_node.h"
#include "memory_pool.h"
#include "types.h"
#include <vector>
#include <cstring>
#include <iostream>
using namespace std;

// each b+ tree node is the size of one block
// so currently 6 keys in 1 node at initialisation
// should be deleting 580 records

int BPlusTree::remove(double minValue, double maxValue) // 0 and 0.35 threshold for deletion experiment
{
    // set numNodes before deletion
    numNodes = index->getAllocated();
    // rmb to get numNodes after deletion

    // troubleshoot empty tree
    if (rootOfTree == nullptr)
    {
        throw std::logic_error("Tree is empty");
    }
    else
    {

        // if tree not empty, load root from the disk
        Address rootDiskAddress{addressOfRootNode, 0};
        rootOfTree = (Node *)index->loadFromDisk(rootDiskAddress, sizeOfNode);

        Node *cursor = rootOfTree;                   // init current node as root
        Node *parent;                          // Keep track of the parent as we go deeper into the tree in case we need to update it.
        void *parentDiskAddress = addressOfRootNode; // Keep track of parent's disk address as well so we can update parent in disk.
        void *cursorDiskAddress = addressOfRootNode; // Store current node's disk address in case we need to update it in disk.
        int leftSibling, rightSibling;         // Index of left and right child to borrow from.

        // Start from root and traverse tree until leaf node
        // Since leaf nodes store ALL key values along w corresponding pointer to disk file,
        // while not leaf, follow node to CORRECT key
        while (cursor->isLeaf == false)
        {
            // set parent of node (jic we need to assign new child) and disk address
            parent = cursor;
            parentDiskAddress = cursorDiskAddress;

            for (int i = 0; i < cursor->numOfKeys; i++) // checking through all keys at current node
            {
                // note left and right sibling in the event of borrowing
                leftSibling = i - 1;
                rightSibling = i + 1;

                // if key in current node is larger than minValue then go the left pointer's node
                if (minValue <= cursor->keys[i])
                {
                    // load node from disk to main mem
                    Node *mainMemoryNode = (Node *)index->loadFromDisk(cursor->pointers[i], sizeOfNode);

                    // update cursorDiskAddress to maintain address on disk if we need to update nodes
                    cursorDiskAddress = cursor->pointers[i].blockAddress;

                    // move to new node in main memory
                    cursor = (Node *)mainMemoryNode;
                    break;
                }
                // check if current key is last key in the node, if yes that means its the largest key in the node
                if (i == cursor->numOfKeys - 1)
                {
                    leftSibling = i;      // no left sib
                    rightSibling = i + 2; // right sib is in the next node

                    // load node from disk to main mem
                        Node *mainMemoryNode = (Node *)index->loadFromDisk(cursor->pointers[i+1], sizeOfNode);
                        // update cursorDiskAddress to maintain address on disk if we need to update nodes
                        cursorDiskAddress = cursor->pointers[i + 1].blockAddress;

                        // Move to new node in main memory
                        cursor = (Node *)mainMemoryNode;
                        break;
                }
            }
        }
        // navigation through the b+ tree (by following appropriate child nodes -- left/right) is COMPLETE
        // we are now at the leaf node that contains the lower threshold
        // search if key we want to delete exists
        int numNodesDeleted;
        // bool found = false;
        int pos;          // position where we find the key
        bool stop = false; // search and delete until key is out or range
            while (stop == false)
        {
            for (pos = 0; pos < cursor->numOfKeys; pos++) // keys within current node
            {
                if (cursor->keys[pos] > maxValue)
                {

                        break; // keys are greater than maxValue, not of interest to us
                }

                if (cursor->keys[pos] >= minValue && cursor->keys[pos] <= maxValue)
                {
                        // key is within range, so we remove it
                        // delete entire linked list before deleting key -- so we don't lose access to LL head
                        // delete linked list stored under key
                        removeLL(cursor->pointers[pos]);
                        // delete key. move keys/pointers forward to replace values
                        for (int i = pos; i < cursor->numOfKeys; i++)
                        {
                            cursor->keys[i] = cursor->keys[i + 1];
                            cursor->pointers[i] = cursor->pointers[i + 1];
                        }
                        cursor->numOfKeys--; // one key has been deleted
                        // set all forward pointers from numKeys onwards to nullputr -- prevents dangling pointers
                        for (int i = cursor->numOfKeys + 1; i < maxKeys + 1; i++)
                        {
                            Address nullAddress{nullptr, 0}; 
                            cursor->pointers[i] = nullAddress;
                        }

                        // update no. of deleted nodes
                        numNodesDeleted++;

                        // keys/pointers have shifted forward so we decrement pos to stay at cur position for next iteration
                        pos--;

                        // in the case where current node is root, check if tree still has keys
                        if (cursor == rootOfTree) 
                        {
                            if (cursor->numOfKeys == 0)
                            {
                                // Delete the entire root node and deallocate it.
                                std::cout << "Entire index has been deleted" << endl;

                                // Deallocate block used to store root node.
                                Address rootDiskAddress{addressOfRootNode, 0};
                                index->deallocate(rootDiskAddress, sizeOfNode);

                                // Reset root pointers in the B+ Tree.
                                rootOfTree = nullptr;
                                addressOfRootNode = nullptr;
                                
                            }
                        std::cout << "Successfully deleted " << cursor->keys[pos] << endl; // will cursor->keys[pos] work? 
                        
                        // update numNodes and numNodesDeleted after deletion
                        int numNodesDeleted = numNodes - index->getAllocated();
                        numNodes = index->getAllocated();

                        // Save to disk.
                        Address cursorAddress = {cursorDiskAddress, 0};
                        index->saveToDisk(cursor, sizeOfNode, cursorAddress);
                        
                        return numNodesDeleted;
                        }

                        // otherwise, balance b+ tree after deleting required keys in current leaf node
                        if (cursor ->numOfKeys >= (maxKeys + 1) / 2)
                        {
                            borrowOrMerge(cursor, parent, leftSibling, rightSibling);
                            pos = 0; //reset pos = 0 to ensure I dont miss out any keys after balancing
                            int numNodesDeleted = numNodes - index->getAllocated();
                            numNodes = index->getAllocated();
                        }
                        else
                        {
                            std::cout << "Successfully deleted" << cursor->keys[pos] << endl; // will cursor->keys[pos] work? 
                            
                            //update numNodes and numNodesDeleted after deletion 
                            int numNodesDeleted = numNodes - index->getAllocated();
                            numNodes = index->getAllocated(); 

                            //save to disk 
                            Address cursorAddress = {cursorDiskAddress, 0};
                            index->saveToDisk(cursor, sizeOfNode, cursorAddress);
                        }
                        

                        

                }
            }
            

            // on last pointer of current leaf node, check if last key is > max
            if (cursor->pointers[cursor->numOfKeys].blockAddress != nullptr && cursor->keys[pos] <= maxValue)
            {
                // set cursor to next leaf node and load from disk
                cursor = (Node *)index->loadFromDisk(cursor->pointers[cursor->numOfKeys], sizeOfNode);
            }
            else
            {
                stop = true; 
            }
        }
    }
    //necessary?
    int numNodesDeleted = numNodes - index->getAllocated(); 
    numNodes = index->getAllocated();
    return numNodesDeleted;
}

// removeInternal function (Add code here)
void BPlusTree::removeInternal(float key, Node *cursorDiskAddress, Node *childDiskAddress)
{
  // Load in cursor (parent) and child from disk to get latest copy.
  Address cursorAddress{cursorDiskAddress, 0};
  Node *cursor = (Node *)index->loadFromDisk(cursorAddress, sizeOfNode);

  // Check if cursor is root via disk address.
  if (cursorDiskAddress == addressOfRootNode)
  {
    rootOfTree = cursor;
  }

  // Get address of child to delete.
  Address childAddress{childDiskAddress, 0};

  // If current parent is root
  if (cursor == rootOfTree)
  {
    // If we have to remove all keys in root (as parent) we need to change the root to its child.
    if (cursor->numOfKeys == 1)
    {
      // If the larger pointer points to child, make it the new root.
      if (cursor->pointers[1].blockAddress == childDiskAddress)
      {
        // Delete the child completely
        index->deallocate(childAddress, sizeOfNode);

        // Set new root to be the parent's left pointer
        // Load left pointer into main memory and update root.
        rootOfTree = (Node *)index->loadFromDisk(cursor->pointers[0], sizeOfNode);
        addressOfRootNode = (Node *)cursor->pointers[0].blockAddress;

        // We can delete the old root (parent).
        index->deallocate(cursorAddress, sizeOfNode);

        // Nothing to save to disk. All updates happened in main memory.
        std::cout << "Root node changed." << endl;
        return;
      }
      // Else if left pointer in root (parent) contains the child, delete from there.
      else if (cursor->pointers[0].blockAddress == childDiskAddress)
      {
        // Delete the child completely
        index->deallocate(childAddress, sizeOfNode);

        // Set new root to be the parent's right pointer
        // Load right pointer into main memory and update root.
        rootOfTree = (Node *)index->loadFromDisk(cursor->pointers[1], sizeOfNode);
        addressOfRootNode = (Node *)cursor->pointers[1].blockAddress;

        // We can delete the old root (parent).
        index->deallocate(cursorAddress, sizeOfNode);

        // Nothing to save to disk. All updates happened in main memory.
        std::cout << "Root node changed." << endl;
        return;
      }
    }
  }

  // If reach here, means parent is NOT the root.
  // Aka we need to delete an internal node (possibly recursively).
  int pos;

  // Search for key to delete in parent based on child's lower bound key.
  for (pos = 0; pos < cursor->numOfKeys; pos++)
  {
    if (cursor->keys[pos] == key)
    {
      break;
    }
  }

  // Delete the key by shifting all keys forward
  for (int i = pos; i < cursor->numOfKeys; i++)
  {
    cursor->keys[i] = cursor->keys[i + 1];
  }

  // Search for pointer to delete in parent
  // Remember pointers are on the RIGHT for non leaf nodes.
  for (pos = 0; pos < cursor->numOfKeys + 1; pos++)
  {
    if (cursor->pointers[pos].blockAddress == childDiskAddress)
    {
      break;
    }
  }

  // Now move all pointers from that point on forward by one to delete it.
  for (int i = pos; i < cursor->numOfKeys + 1; i++)
  {
    cursor->pointers[i] = cursor->pointers[i + 1];
  }

  // Update numKeys
  cursor->numOfKeys--;

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