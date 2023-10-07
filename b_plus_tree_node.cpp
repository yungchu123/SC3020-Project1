#include "b_plus_tree_node.h"

BPlusTreeNode::BPlusTreeNode(int maxKeys)
{
    this->keys = new float[maxKeys];
    this->pointers = new void *[maxKeys + 1]
    { nullptr };
}