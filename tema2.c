#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdint.h>
#include "StackQueue.h"

typedef struct _Thread
{
    int8_t thread;
} Thread;

typedef struct _Task
{
    int16_t id;
    int8_t thread;
    int8_t priority;
    int time;
    int initial_time;
} Task;

void freeEL2(void* x)
{
    // da free la o structura de tip task
    Task* y = (Task*)x;
    free(y);
}

void removeID(Stack** ids, void (*freeEL)(void*))
{
    if((*ids)->size == 1) {
        int16_t* i = (int16_t*) (*ids)->top->value;
        (*i)++;  // incrementeaza valorea din stiva
        (*ids)->top->value = (*ids)->bottom->value = (void*)i;
    }
    else {
        pop((*ids), freeEL);  // sterge valoarea din stiva
    }
}

int compareIDstiva(void* x, void* y)
{
    int16_t a = *(int16_t*)x;
    int16_t b = *(int16_t*)y;
    if(a > b) return 1;
    if(a == b) return 0;
    return -1;
}

int comparePriority(void* x, void* y)
{
    Task* a = (Task*)x;
    Task* b = (Task*)y;
    if(a->priority > b->priority) return 1;
    if(a->priority == b->priority) return 0;
    return -1;
}

int compareTime(void* x, void* y)
{
    Task* a = (Task*)x;
    Task* b = (Task*)y;
    if(a->time > b->time) return 1;
    if(a->time == b->time) return 0;
    return -1;
}

int compareID(void* x, void* y)
{
    Task* a = (Task*)x;
    Task* b = (Task*)y;
    if(a->id > b->id) return 1;
    if(a->id == b->id) return 0;
    return -1;
}

int compareThread(void* x, void* y)
{
    Task* a = (Task*)x;
    Task* b = (Task*)y;
    if(a->thread > b->thread) return 1;
    if(a->thread == b->thread) return 0;
    return -1;
}

void add_tasks(FILE* fout, Stack** ids, Queue** waitingQueue, int time,
  int8_t priority, void (*freeEL)(void*), int (*comparePriority)(void*, void*),
  int (*compareTime)(void*, void*), int (*compareID)(void*, void*))
{
    // creeaza taskul si sterge id-ul care i-a fost atribuit din stiva
    Task* task = calloc(1, sizeof(struct _Task));
    if(task == NULL) return;
    task->time = time;
    task->initial_time = time;
    task->priority = priority;
    task->id = *(int16_t*) (*ids)->top->value;
    task->thread = -1;
    removeID(ids, freeEL);

    // parcurge coada pana gaseste locul unde trebuie inserat taskul
    // in functie de specificatii
    Queue* auxq = CreateQueue();
    while (!EmptyQueue(*waitingQueue) &&
      (comparePriority((*waitingQueue)->front->value, (void*)task) > 0 ||
      (comparePriority((*waitingQueue)->front->value, (void*)task) == 0 &&
      compareTime((*waitingQueue)->front->value, (void*)task) < 0) ||
      (comparePriority((*waitingQueue)->front->value, (void*)task) == 0 &&
      compareTime((*waitingQueue)->front->value, (void*)task) == 0 &&
      compareID((*waitingQueue)->front->value, (void*)task) < 0)))
    {
        enqueue(auxq, (*waitingQueue)->front->value);
        dequeue((*waitingQueue), freeEL);
    }


    // insereaza taskul in coada de waiting
    Node* aux = CreateNode((*waitingQueue)->front, (void*)task);
    if(aux == NULL) return;
    (*waitingQueue)->front = aux;
    (*waitingQueue)->size++;
    
    fprintf(fout, "Task created successfully : ID %d.\n", task->id);

    // pune restul de elemente ramase in coada waiting in coada auxiliara
    while(!EmptyQueue(*waitingQueue))
    {
        enqueue(auxq, (*waitingQueue)->front->value);
        dequeue((*waitingQueue), freeEL);
    }

    // reface coada initiala cu taskul adaugat
    while(!EmptyQueue(auxq))
    {
        enqueue((*waitingQueue), auxq->front->value);
        dequeue(auxq, freeEL);
    }

    freeQueue(&auxq, freeEL2);

    return;
}

Task* findRunning(Queue* runningQueue, int16_t id,
  int (*compareID)(void*, void*), void (*freeEL)(void*))
{
    // un task auxiliar care contine id-ul dat pentru a putea compara id-urile
    Task* aux = calloc(1, sizeof(struct _Task));
    if(aux == NULL) return NULL;
    aux->time = 0;
    aux->priority = 0;
    aux->id = id;
    aux->thread = -1;

    Task* returneaza = NULL;
    Queue* auxq = CreateQueue();

    // cauta in coada id-ul in coada
    while(!EmptyQueue(runningQueue)) {
        if(compareID(runningQueue->front->value, (void*)aux) == 0)
            returneaza = (Task*)runningQueue->front->value;

        enqueue(auxq, runningQueue->front->value);
        dequeue(runningQueue, freeEL);
    }

    // reface coada
    while(!EmptyQueue(auxq)) {
        enqueue(runningQueue, auxq->front->value);
        dequeue(auxq, freeEL);
    }

    freeQueue(&auxq, freeEL2);
    free(aux);
    return returneaza;
}

Task* findWaiting(Queue* waitingQueue, int16_t id,
  int (*compareID)(void*, void*), void (*freeEL)(void*))
{
    Task* aux = calloc(1, sizeof(struct _Task));
    aux->time = 0;
    aux->priority = 0;
    aux->id = id;
    aux->thread = -1;

    Task* returneaza = NULL;
    Queue* auxq = CreateQueue();
    while(!EmptyQueue(waitingQueue)) {
        if(compareID(waitingQueue->front->value, (void*)aux) == 0)
            returneaza = (Task*)waitingQueue->front->value;

        enqueue(auxq, waitingQueue->front->value);
        dequeue(waitingQueue, freeEL);
    }

    while(!EmptyQueue(auxq)) {
        enqueue(waitingQueue, auxq->front->value);
        dequeue(auxq, freeEL);
    }
    freeQueue(&auxq, freeEL2);
    free(aux);
    return returneaza;
}

Task* findFinished(Queue* finishedQueue, int16_t id,
  int (*compareID)(void*, void*), void (*freeEL)(void*))
{
    Task* aux = calloc(1, sizeof(struct _Task));
    aux->id = id;

    Task* returneaza = NULL;
    Queue* auxq = CreateQueue();
    while(!EmptyQueue(finishedQueue)) {
        if(compareID(finishedQueue->front->value, (void*)aux) == 0)
            { returneaza = (Task*)finishedQueue->front->value; break; }

        enqueue(auxq, finishedQueue->front->value);
        dequeue(finishedQueue, freeEL);
    }

    while(!EmptyQueue(finishedQueue)) {
        enqueue(auxq, finishedQueue->front->value);
        dequeue(finishedQueue, freeEL);
    }

    while(!EmptyQueue(auxq)) {
        enqueue(finishedQueue, auxq->front->value);
        dequeue(auxq, freeEL);
    }
    freeQueue(&auxq, freeEL2);
    free(aux);
    return returneaza;
}

void get_task(FILE *fout, Queue* runningQueue, Queue* waitingQueue,
  Queue* finishedQueue, int16_t id, int (*compare)(void*, void*),
  void (*freeEL)(void*))
{
    if(findRunning(runningQueue, id, compare, freeEL) != NULL) {
        Task* gasit = findRunning(runningQueue, id, compare, freeEL);
        fprintf(fout, "Task %d is running (remaining_time = %d).\n",
          id, gasit->time);
        return;
    }
    if(findWaiting(waitingQueue, id, compare, freeEL) != NULL) {
        Task* gasit = findWaiting(waitingQueue, id, compare, freeEL);
        fprintf(fout, "Task %d is waiting (remaining_time = %d).\n",
          id, gasit->initial_time);
        return;
    }
    if(findFinished(finishedQueue, id, compare, freeEL) != NULL) {
        Task* gasit = findFinished(finishedQueue, id, compare, freeEL);
        fprintf(fout, "Task %d is finished (executed_time = %d).\n",
          gasit->id, gasit->initial_time);
        return;
    }
    fprintf(fout, "Task %d not found.\n", id);
}

void get_thread(FILE *fout, Stack* stack, Queue* runningQueue, int8_t id,
  int (*compare)(void*, void*), void (*freeEL)(void*))
{
    // task auxiliar care contine id-ul dat
    Task* aux = calloc(1, sizeof(struct _Task));
    aux->time = 0;
    aux->priority = 0;
    aux->id = -1;
    aux->thread = id;
    int ok = 0;

    Stack* auxs = CreateStack();
    while(!EmptyStack(stack)) {
        Task* top = calloc(1, sizeof(struct _Task));
        top->time = 0;
        top->priority = 0;
        top->id = -1;
        top->thread = *(int8_t*)stack->top->value;
        // top contine id-ul threadului din topul stivei
        
        // verifica daca threadul dat este idle in stiva
        if(compare(top, aux) == 0) { fprintf(fout, "Thread %d is idle.\n", id);
          ok = 1; free(top); break; }

        push(auxs, stack->top->value);
        pop(stack, freeEL);
        free(top);
    }

    // baga elementele scoase inapoi in stiva
    while(!EmptyStack(auxs)) {
        push(stack, auxs->top->value);
        pop(auxs, freeEL);
    }

    // daca threadul e idle iese din functie
    if(ok) {
        free(aux);
        freeStack(&auxs, free);
        return;
    }

    // cauta threadul in running
    Queue* auxq = CreateQueue();
    while(!EmptyQueue(runningQueue)) {
        if(compare(runningQueue->front->value, (void*)aux) == 0) {
        Task* t = (Task*) runningQueue->front->value;
            fprintf(fout, "Thread %d is running task %d (remaining_time = %d).\n",
              id, t->id, t->time);
        }

        enqueue(auxq, runningQueue->front->value);
        dequeue(runningQueue, freeEL);
    }
    free(aux);

    // reface coada de running
    while(!EmptyQueue(auxq)) {
        enqueue(runningQueue, auxq->front->value);
        dequeue(auxq, freeEL);
    }
    freeQueue(&auxq, freeEL2);
    freeStack(&auxs, free);
}

void printWaiting(FILE *fout, Queue* waitingQueue, void (*freeEL)(void*))
{
    fprintf(fout, "====== Waiting queue =======\n[");
    Queue* aux = CreateQueue();
    while(!EmptyQueue(waitingQueue)) {
        Task* t = (Task*) waitingQueue->front->value;
        fprintf(fout, "(%d: priority = %d, remaining_time = %d)",
          t->id, t->priority, t->initial_time);
        if(waitingQueue->front->next != NULL) fprintf(fout, ",\n");
        
        enqueue(aux, waitingQueue->front->value);
        dequeue(waitingQueue, freeEL);
    }

    fprintf(fout, "]\n");

    // reface coada
    while(!EmptyQueue(aux)) {
        enqueue(waitingQueue, aux->front->value);
        dequeue(aux, freeEL);
    }
    freeQueue(&aux, freeEL2);
}

void printRunning(FILE *fout, Queue* runningQueue, void (*freeEL)(void*))
{
    fprintf(fout, "====== Running in parallel =======\n[");
    Queue* aux = CreateQueue();
    while(!EmptyQueue(runningQueue)) {
        Task* t = (Task*) runningQueue->front->value;
        fprintf(fout,
          "(%d: priority = %d, remaining_time = %d, running_thread = %d)",
          t->id, t->priority, t->time, t->thread);
        if(runningQueue->front->next != NULL) fprintf(fout, ",\n");

        enqueue(aux, runningQueue->front->value);
        dequeue(runningQueue, freeEL);
    }

    fprintf(fout, "]\n");

    // reface coada
    while(!EmptyQueue(aux)) {
        enqueue(runningQueue, aux->front->value);
        dequeue(aux, freeEL);
    }
    freeQueue(&aux, freeEL2);
}

void printFinished(FILE *fout, Queue* finishedQueue, void (*freeEL)(void*))
{
    fprintf(fout, "====== Finished queue =======\n[");
    Queue* aux = CreateQueue();
    while(!EmptyQueue(finishedQueue)) {
        Task* t = (Task*) finishedQueue->front->value;
        fprintf(fout, "(%d: priority = %d, executed_time = %d)",
          t->id, t->priority, t->initial_time);
        if(finishedQueue->front->next != NULL) fprintf(fout, ",\n");

        enqueue(aux, finishedQueue->front->value);
        dequeue(finishedQueue, freeEL);
    }
    
    fprintf(fout, "]\n");

    // reface coada
    while(!EmptyQueue(aux)) {
        enqueue(finishedQueue, aux->front->value);
        dequeue(aux, freeEL);
    }
    freeQueue(&aux, freeEL2);
}

int minim(int a, int b)
{
    if(a <= b) return a;
    return b;
}

int maxim(Queue* runningQueue, void (*freeEL)(void*)) {
    // intoarce maximul de timp din coada
    Queue* aux = CreateQueue();
    int max = 0;
    while(!EmptyQueue(runningQueue)) {
        Task* t = (Task*) runningQueue->front->value;
        if(max < t->time) max = t->time;

        enqueue(aux, runningQueue->front->value);
        dequeue(runningQueue, freeEL);
    }

    // reface coada
    while(!EmptyQueue(aux)) {
        enqueue(runningQueue, aux->front->value);
        dequeue(aux, freeEL);
    }
    freeQueue(&aux, freeEL2);
    return max;
}

void run(int Q, int T, int* timpRun, Queue* waitingQueue, Queue* runningQueue,
  Queue* finishedQueue, Stack* stack, Stack* ids, void (*freeEL)(void*),
  int (*compareIDstiva)(void*, void*))
{
    int rezerva = 0;  // retine cate secunde raman in plus dupa ce
    // s-au terminat taskurile dar a mai ramas timp pentru a sti
    // sa nu le adauge la timp

    // adauga taskurile din waiting in running
    while(!EmptyQueue(waitingQueue) && !EmptyStack(stack)) {
        int8_t thread = *(int8_t*) stack->top->value;
        Task* task = (Task*) waitingQueue->front->value;
        pop(stack, freeEL);
        dequeue(waitingQueue, freeEL);
        task->thread = thread;  // atribuie un thread din stiva taskului
        enqueue(runningQueue, (void*)task);
    }

    int contor = 0;  // retine de cate ori trebuie sa faca bucla
    if(T % Q == 0) contor = T/Q;
    else contor = T/Q + 1;

    while(contor > 0) {
        Queue* auxRun = CreateQueue();
        int min = minim(T, Q);
        int maxRun = maxim(runningQueue, freeEL);
        if(maxRun < min) min = maxRun;
        // se adauga la timp minimul dintre T, Q si maximul din coada running
        *timpRun += min;
        T -= min;
        
        // scade timpul din taskurile din runningQueue
        while(!EmptyQueue(runningQueue)) {
            Task* t = (Task*) runningQueue->front->value;
            t->time -= min;
            if(t->time < min) rezerva = min - t->time - Q;
            enqueue(auxRun, (void*)t);
            dequeue(runningQueue, freeEL);
        }

        while(!EmptyQueue(auxRun)) {
            Task* t = (Task*) auxRun->front->value;
            int ok = 0;
            if(t->time <= 0) {
                // ia id-ul taskului care s-a terminat si il
                // adauga in stiva de id-uri pentru taskuri
                Stack* aux = CreateStack();

                // parcurge pana in locul unde trebuie adaugat
                // taskul astfel incat sa fie in ordine crescatoare
                while (ids->size != 1 &&
                  compareIDstiva(ids->top->value, (void*)&t->id) < 0) {
                    push(aux, ids->top->value);
                    pop(ids, freeEL);
                }

                // insereaza ID-ul
                if(compareIDstiva(ids->top->value, (void*)&t->id) != 0)
                    push(ids, (void*)&t->id);

                // reface stiva initiala cu ID-ul adaugat
                while(!EmptyStack(aux))
                {
                    push(ids, aux->top->value);
                    pop(aux, freeEL);
                }

                // adauga taskul in finished
                enqueue(finishedQueue, auxRun->front->value);
                dequeue(auxRun, freeEL);
                // adauga id-ul threadului in stiva
                push(stack, (void*)&t->thread);
                ok = 1;  // taskul s-a terminat
                freeStack(&aux, freeEL);
            }
            if(ok == 0) {
                // daca taskul nu s-a terminat se baga inapoi in running
                enqueue(runningQueue, auxRun->front->value);
                dequeue(auxRun, freeEL);
            }
           
        }

        // baga taskuri in running pana cand nu mai sunt
        // threaduri sau nu mai sunt taskuri
        while(!EmptyQueue(waitingQueue) && !EmptyStack(stack)) {
            int8_t thread = *(int8_t*) stack->top->value;
            Task* task = (Task*) waitingQueue->front->value;
            if(task->time < min) rezerva = min - task->time - Q;
            pop(stack, freeEL);
            dequeue(waitingQueue, freeEL);
            task->thread = thread;
            enqueue(runningQueue, (void*)task);
        }

        if(EmptyQueue(runningQueue) && EmptyQueue(waitingQueue) || T <= 0) {
            freeQueue(&auxRun, freeEL2);
            return;
        } 

        contor--;
        freeQueue(&auxRun, freeEL2);
    }
}

int finish(int Q, Stack* stack, Queue* runningQueue, Queue* waitingQueue,
  Queue* finishedQueue, void (*freeEL)(void*))
{
    int max = 0;
    int t = 0;
    int rezerva = 0;

    // adauga taskuri in running
    while(!EmptyQueue(waitingQueue) && !EmptyStack(stack)) {
        Task* t = (Task*)waitingQueue->front->value;
        if(t->time > max) max = t->time;
        pop(stack, freeEL);
        dequeue(waitingQueue, freeEL);
        enqueue(runningQueue, (void*)t);
    }

    while(max > 0) {
        Queue* auxRun = CreateQueue();

        // scade timpul din taskurile din runningQueue
        while(!EmptyQueue(runningQueue)) {
            Task* t = (Task*) runningQueue->front->value;
            t->time -= Q;
            if(t->time < Q  && t->time > 0) rezerva = Q - t->time;
            enqueue(auxRun, (void*)t);
            dequeue(runningQueue, freeEL);
        }

        while(!EmptyQueue(auxRun)) {
            Task* t = (Task*) auxRun->front->value;
            int ok = 0;
            if(t->time <= 0) {
                enqueue(finishedQueue, auxRun->front->value);
                dequeue(auxRun, freeEL);
                push(stack, (void*)&t->thread);
                ok = 1;
            }
            if(ok == 0) {
                enqueue(runningQueue, auxRun->front->value);
                dequeue(auxRun, freeEL);
            }
        }

        // baga taskuri in running pana cand nu mai sunt
        // threaduri sau nu mai sunt taskuri
        while(!EmptyQueue(waitingQueue) && !EmptyStack(stack)) {
            Task* t = (Task*)waitingQueue->front->value;
            if(t->time > max) max = t->time;
            if(t->time < Q && t->time > 0) rezerva = Q - t->time;
            pop(stack, freeEL);
            dequeue(waitingQueue, freeEL);
            enqueue(runningQueue, (void*)t);
        }

        if(EmptyQueue(runningQueue) && EmptyQueue(waitingQueue))
        { freeQueue(&auxRun, freeEL2); t += max; return t; }
        freeQueue(&auxRun, freeEL2);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // CITIREA
    FILE *f = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");
    char *linie = calloc(1000, sizeof(char));
    char *cerinta;

    int Q;
    int8_t C;
    int8_t N;
    int timpRun = 0;
    int timpTotal = 0;
    fgets(linie, 1000, f);
    linie[strcspn(linie, "\r")] = 0;
    linie[strcspn(linie, "\n")] = 0;
    Q = atoi(linie);
    fgets(linie, 1000, f);
    linie[strcspn(linie, "\r")] = 0;
    linie[strcspn(linie, "\n")] = 0;
    int c = atoi(linie);
    C = (int8_t) c;
    N = 2 * C;

    // adauga id-urile threadurilor in stiva
    Stack* stack = CreateStack();
    int8_t* v = (int8_t*)malloc(N * sizeof(int8_t));
    for(int i = 0; i < N; i++) {
        v[i] = N - 1 - i;
        push(stack, (void*)(&v[i]));
    }

    // adauga un element in stiva de id-uri care va fi "contorul" taskurilor
    Stack* ids = CreateStack();
    int16_t x = 1;
    ids->top = ids->bottom = CreateNode(NULL, (void*)&x);
    ids->size++;

    Queue* waitingQueue = CreateQueue();
    Queue* runningQueue = CreateQueue();
    Queue* finishedQueue = CreateQueue();

    while (fgets(linie, 1000, f) != NULL)
    {
        linie[strcspn(linie, "\r")] = 0;
        linie[strcspn(linie, "\n")] = 0;
        cerinta = strtok(linie, " ");
        if (strcmp(cerinta, "add_tasks") == 0)
        {
            cerinta = strtok(NULL, " ");
            int nr = atoi(cerinta);
            cerinta = strtok(NULL, " ");
            int time = atoi(cerinta);
            cerinta = strtok(NULL, " ");
            int p = atoi(cerinta);
            int8_t priority = (int8_t) p;
            while(nr) {
                add_tasks(fout, &ids, &waitingQueue, time, priority,
                  freeEL, comparePriority, compareTime, compareID);
                nr--;
            }
        }
        if (strcmp(cerinta, "get_task") == 0)
        {
            cerinta = strtok(NULL, " ");
            int i = atoi(cerinta);
            int16_t id = (int16_t) i;
            get_task(fout, runningQueue, waitingQueue, finishedQueue,
              id, compareID, freeEL);
        }
        if (strcmp(cerinta, "get_thread") == 0)
        {
            cerinta = strtok(NULL, " ");
            int i = atoi(cerinta);
            int8_t id = (int8_t) i;
            get_thread(fout, stack, runningQueue, id, compareThread, freeEL);
        }
        if (strcmp(cerinta, "print") == 0)
        {
            cerinta = strtok(NULL, " ");
            if (strcmp(cerinta, "waiting") == 0)
            {
                printWaiting(fout, waitingQueue, freeEL);
            }
            if (strcmp(cerinta, "running") == 0)
            {
                printRunning(fout, runningQueue, freeEL);
            }
            if (strcmp(cerinta, "finished") == 0)
            {
                printFinished(fout, finishedQueue, freeEL);
            }
        }
        if (strcmp(cerinta, "run") == 0)
        {
            cerinta = strtok(NULL, " ");
            int T = atoi(cerinta);
            fprintf(fout, "Running tasks for %d ms...\n", T);
            run(Q, T, &timpRun, waitingQueue, runningQueue, finishedQueue,
              stack, ids, freeEL, compareIDstiva);
        }
        if (strcmp(cerinta, "finish") == 0)
        {
            timpTotal = finish(Q, stack, runningQueue, waitingQueue,
              finishedQueue, freeEL);
            timpTotal += timpRun;
            fprintf(fout, "Total time: %d", timpTotal);
        }
    }
    
    // DEZALOCARE
    freeStack(&stack, freeEL);
    freeStack(&ids, freeEL);
    freeQueue(&waitingQueue, freeEL2);
    freeQueue(&runningQueue, freeEL2);
    freeQueue(&finishedQueue, freeEL2);
    free(linie);
    free(v);
    fclose(f);
    fclose(fout);
}
