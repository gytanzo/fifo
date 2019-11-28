#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "csesem.h"
#include "pcq.h"

/* This structure must contain everything you need for an instance of a
 * PCQueue.  The given definition is ABSOLUTELY NOT COMPLETE.  You will
 * have to add several items to this structure. */
struct PCQueue {
    void **queue;
    int slots;
    void *first;
    void *last;
    CSE_Semaphore mutex;
    CSE_Semaphore open;
    CSE_Semaphore items;
};

/* The given implementation performs no error checking and simply
 * allocates the queue itself.  You will have to create and initialize
 * (appropriately) semaphores, mutexes, condition variables, flags,
 * etc. in this function. */
PCQueue pcq_create(int n) {
    PCQueue pcq;
    pcq = calloc(1, sizeof(*pcq));

    void ***queue = &(pcq -> queue);
    int *slots = &(pcq -> slots);
    void **first = &(pcq -> first);
    void **last = &(pcq -> last);
    CSE_Semaphore *mutex = &(pcq -> mutex);
    CSE_Semaphore *open = &(pcq -> open);
    CSE_Semaphore *items = &(pcq -> items);

    *queue = calloc(n, sizeof(void *));
    *slots = n;
    *first = *last = 0;
    *mutex = csesem_create(1);
    *open = csesem_create(n);
    *items = csesem_create(0);

    int check_one = (*queue != NULL);
    int check_two = (*mutex != NULL);
    int check_three = (*open != NULL);
    int check_four = (*items != NULL);

    if ((check_one && check_two && check_three && check_four) != 1){
       puts("ERR: An initialization function failed. Check below.");
       if (check_one == 0){
           puts("*queue initialization failed.");
       }
       if (check_two == 0){
           puts("*mutex initialization failed.");
       }
       if (check_three == 0){
           puts("*open initialization failed.");
       }
       if (check_four == 0){
           puts("*items initialization failed.");
       }
       return NULL;
    }

    return pcq;
}

/* This implementation does nothing, as there is not enough information
 * in the given struct PCQueue to even usefully insert a pointer into
 * the data structure. */
void pcq_insert(PCQueue pcq, void *data) {
}

/* This implementation does nothing, for the same reason as
 * pcq_insert(). */
void *pcq_retrieve(PCQueue pcq) {
    return NULL;
}

/* The given implementation blindly frees the queue.  A minimal
 * implementation of this will need to work harder, and ensure that any
 * synchronization primitives allocated here are destroyed; a complete
 * and correct implementation will have to synchronize with any threads
 * blocked in pcq_create() or pcq_destroy().
 *
 * You should implement the complete and correct clean teardown LAST.
 * Make sure your other operations work, first, as they will be tightly
 * intertwined with teardown and you don't want to be debugging it all
 * at once!
 */
void pcq_destroy(PCQueue pcq) {
    free(pcq->queue);
    free(pcq);
}
