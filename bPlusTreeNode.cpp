#include "bPlusTreeNode.h"

bPlusTreeNode::bPlusTreeNode(int maxKeys)
{
    this->keys = new float[maxKeys];
    this->pointers = new Address[maxKeys + 1];

    for (int i = 0; i < maxKeys; i++)
    {
        Address nullAddress{nullptr, 0};
        pointers[i] = nullAddress;
    }
}