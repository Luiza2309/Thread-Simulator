#include "StackQueue.h"
#include <stdio.h>

void freeEL(void* x)
{
    
}




// STACK

Stack* CreateStack()
{
    Stack* stack = calloc(1, sizeof(struct _Stack));
    stack->bottom = stack->top = NULL;
    stack->size = 0;
    return stack;
}

Stack* CreateSimpleStack(short int N)
{
    Stack* stack = CreateStack();
    while(N) {
        int aux = N - 1;
        push(stack, (void*)&aux);
        // printf("%d ", *(int*)stack->top->value);
        N--;
    }

    return stack;
}

int EmptyStack(Stack* s)
{
    if(s->size == 0) return 1;
    return 0;
}

void push(Stack* stack, void* value)
{
    if( stack->size == 0 )
    {
        stack->bottom = stack->top = CreateNode(NULL, value);
        stack->size++;
        return;
    }

    Node* n = CreateNode(NULL, value);
    stack->top->next = n;
    stack->top = n;
    stack->size++;
}

void pop(Stack* stack, void (*freeEl)(void*))
{
    if( stack->size == 1 )
    {
        freeEl(stack->top->value);
        free(stack->top);
        stack->bottom = stack->top = NULL;
        stack->size = 0;
        return;
    }

    Node* n = stack->bottom;
    while( n != NULL && n->next != stack->top )
        n =  n->next;

    Node* topNode = stack->top;
    stack->top = n;

    freeEl(topNode->value);
    free(topNode);
    stack->size--;
}

void printStack(Stack* s, void (*freeEL)(void*))
{
    if (EmptyStack(s))
        return;
    Stack* aux = CreateStack();
    while(!EmptyStack(s)) {
        printf("%d ", *(char*) s->top->value);
        push(aux, s->top->value);
        pop(s, freeEL);
    }

    while(!EmptyStack(aux)) {
        push(s, aux->top->value);
        pop(aux, freeEL);
    }
}

void freeStack(Stack** stack, void (*freeEl)(void*))
{
    while( ! EmptyStack(*stack) )
        pop(*stack, freeEl);
    free(*stack);
    *stack = NULL;
}

// QUEUE

Queue* CreateQueue()
{
    Queue* q = calloc(1, sizeof(struct _Queue));
    q->back = q->front = NULL;
    q->size = 0;
    return q;
}

int EmptyQueue(Queue* q)
{
    if(q->size == 0) return 1;
    return 0;
}

void enqueue(Queue* q, void* value)
{
    if( q->back == NULL )
    {
        q->back = q->front = CreateNode(NULL, value);
        q->size++;
        return;
    }

    Node* n = CreateNode(NULL, value);
    q->back->next = n;
    q->back = n;
    q->size++;
}

void dequeue(Queue* q, void (*freeEL)(void*))
{
    if( q->size == 1 )
    {
        freeEL(q->front->value);
        free(q->front);
        q->front = q->back = NULL;
        q->size = 0;
        return;
    }

    Node* frontNode = q->front;
    q->front = frontNode->next;

    freeEL(frontNode->value);
    free(frontNode);
    q->size--;
}

void freeQueue(Queue** q, void (*freeEL)(void*))
{
    while( !EmptyQueue(*q) )
        dequeue(*q, freeEL);
    free(*q);
    *q = NULL;
}