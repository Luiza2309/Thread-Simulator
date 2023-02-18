#include "Node.h"

Node* CreateNode(Node* next, void* value)
{
    Node* newNode = calloc(1, sizeof(struct _Node));
    newNode->next = next;
    newNode->value = value;
    return newNode;
}

