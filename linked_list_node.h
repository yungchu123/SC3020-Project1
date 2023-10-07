#ifndef LINKED_LIST_NODE_H
#define LINKED_LIST_NODE_H

#include "types.h"

class LLNode
{
    private:
        Address recordAddress;
        LLNode *next;
        friend class LL;

public:
    LLNode(Address recordAddress);
    
    Address getRecordAddress()
    {
        return recordAddress;
    }

    LLNode* getNext()
    {
        return next;
    }
};

class LL
{
private:
    LLNode *head;
    float key;
    int numRecords;
    friend class LL;

public:
    LL(LLNode *head, float key);

    void insert(Address recordAddress);

    LLNode* getHead()
    {
        return head;
    }

    int getNumRecords()
    {
        return numRecords;
    }

    float getKey()
    {
        return key;
    }
};

#endif