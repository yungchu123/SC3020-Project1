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

    //changed delete to LLdelete
    void LLdelete();
    
    //changed delete to LLdelete
    ~LL(){
        LLdelete();
    }
};

#endif