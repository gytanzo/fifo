#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

#include "csesem.h"

/* This definition of struct CSE_Semaphore is only available _inside_
 * your semaphore implementation.  This prevents calling code from
 * inadvertently invalidating the internal representation of your
 * semaphore.  See csesem.h for more information.
 *
 * You may place any data you require in this structure. */
struct CSE_Semaphore {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t entered;
    pthread_cond_t done;
};

/* This function must both allocate space for the semaphore and perform
 * any initialization that is required for safe operation on the
 * semaphore. The user should be able to immediately call csesem_post()
 * or csesem_wait() after this routine returns. */
CSE_Semaphore csesem_create(int count) {
    CSE_Semaphore sem = calloc(1, sizeof(struct CSE_Semaphore));
    int *value = &(sem -> value);
    pthread_mutex_t *mutex = &(sem -> mutex);
    pthread_cond_t *done = &(sem -> done);

    *value = count;
    if (*value < 0){
        puts("ERR: Count parameter isn't nonnegative.");
        return NULL;
    }

    int check_one = pthread_mutex_init(mutex, NULL);
    int check_two = pthread_cond_init(done, NULL);

    if (check_one != 0 || check_two != 0){
        puts("ERR: An initialization function failed.");
        return NULL;
    }
    
    return sem;
}

void csesem_wait(CSE_Semaphore sem) {
    int *value = &(sem -> value);
    pthread_mutex_t *mutex = &(sem -> mutex);
    pthread_cond_t *done = &(sem -> done);

    if (*value > 0){
        *value = *value - 1;
        return;
    }
    else {
        pthread_mutex_lock(mutex);
        while (*value == 0){
            pthread_cond_wait(done, mutex);
        }
        pthread_mutex_unlock(mutex);
        return;
    }
}

void csesem_post(CSE_Semaphore sem) {
    int *value = &(sem -> value);
    pthread_cond_t *done = &(sem -> done);
    pthread_mutex_t *mutex = &(sem -> mutex);

    if (!mutex){
        ;
    }
    *value = *value + 1;
    pthread_cond_signal(done);
}


/* This function should destroy any resources allocated for this
 * semaphore; this includes mutexes or condition variables. */
void csesem_destroy(CSE_Semaphore sem) {
    free(sem);
}
