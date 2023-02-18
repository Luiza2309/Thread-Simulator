#include "Node.h"

typedef struct _Stack
{
    Node* top;
    Node* bottom;
    int size;
}Stack;

void freeEL(void* x);
Stack* CreateStack();
Stack* CreateSimpleStack(short int N);
void freeStack(Stack** stack, void (*freeEL)(void*));
int EmptyStack(Stack* s);
void printStack(Stack* s, void (*freeEL)(void*));
void push(Stack* stack, void* value);
void pop(Stack* stack, void (*freeEL)(void*));


typedef struct _Queue
{
    Node* front;
    Node* back;
    int size;
}Queue;

Queue* CreateQueue();
void freeQueue(Queue** q, void (*freeEL)(void*));
void enqueue(Queue* q, void* value);
void dequeue(Queue* q, void (*freeEL)(void*));
int EmptyQueue(Queue* q);