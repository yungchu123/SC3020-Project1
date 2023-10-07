#ifndef LINKED_LIST_NODE_H
#define LINKED_LIST_NODE_H

#include "types.h"

class LLNode
{
    friend class LL;

private:
    Address recordAddress;
    LLNode *next;

public:
    LLNode(Address recordAddress);
};

class LL
{
private:
    LLNode *head;
    float key;
    int numRecords;

public:
    LL(LLNode *head, float key);

    void insert(Address recordAddress);
};

#endif