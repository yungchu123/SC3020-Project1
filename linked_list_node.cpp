#include "linked_list_node.h"
#include "types.h"

LLNode::LLNode(Address recordAddress)
{
    this->recordAddress = recordAddress;
    this->next = nullptr;
}

LL::LL(LLNode *head, float key)
{
    this->numRecords = 0;
    this->head = head;
    this->key = key;
}

void LL::insert(Address recordAddress)
{
    LLNode *tempNode = new LLNode(recordAddress);
    tempNode->next = this->head;
    this->head = tempNode;
    this->numRecords++;
}

//changed delete to LLdelete
void LL::LLdelete(){
    LLNode* current = this->head;
    LLNode* nextNode;

    while (current != nullptr)
    {
        nextNode = current->next;
        delete current;
        current = nextNode;
    }
    this->head = nullptr;
}