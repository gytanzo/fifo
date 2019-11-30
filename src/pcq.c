#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "csesem.h"
#include "pcq.h"

struct PCQueue {
    void **queue;
    int slots;
    int available;
    CSE_Semaphore mutex;
    CSE_Semaphore open;
    CSE_Semaphore items;
};

PCQueue pcq_create(int n) {
    PCQueue pcq = calloc(1, sizeof(*pcq));

    void ***queue = &(pcq -> queue);
    int *slots = &(pcq -> slots);
    int *available = &(pcq -> available);
    CSE_Semaphore *mutex = &(pcq -> mutex);
    CSE_Semaphore *open = &(pcq -> open);
    CSE_Semaphore *items = &(pcq -> items);

    *queue = calloc(n, sizeof(void *));
    *slots = *available = n;
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

void pcq_insert(PCQueue pcq, void *data) {
    csesem_wait(pcq->open);
    csesem_wait(pcq->mutex);

    int *slots = &(pcq -> slots);
    int *available = &(pcq -> available);

    if (*available == *slots){
        int index = *slots - *available;
        pcq->queue[index] = data;
        *available = *available - 1;
    }

    else {
        if (*available >= 1){
            int index = *slots - *available;
            pcq->queue[index] = data;
            *available = *available - 1;
        }
    }

    csesem_post(pcq->mutex);
    csesem_post(pcq->items);
}

void *pcq_retrieve(PCQueue pcq) { 
    csesem_wait(pcq->items);
    csesem_wait(pcq->mutex);

    int *slots = &(pcq -> slots);
    int *available = &(pcq -> available);
    int index = 0;
    void *data = NULL;

    data = pcq->queue[0];
    *available = *available + 1;

    while (index != *slots){
        if (index + 1 != *slots){
            pcq->queue[index] = pcq->queue[index + 1];
        }
        else {
            pcq->queue[index] = NULL;
        }
        index++;
    }
    
    csesem_post(pcq->mutex);
    csesem_post(pcq->open);
    
    return data;
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
    csesem_destroy(pcq->items);
    csesem_destroy(pcq->mutex);
    csesem_destroy(pcq->open);

    free(pcq->queue);
    free(pcq);
}
