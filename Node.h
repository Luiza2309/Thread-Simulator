#include <string.h>
#include <stdlib.h>

typedef struct _Node
{
    void* value;
    struct _Node* next;
}Node;

Node* CreateNode(Node* next, void* value);