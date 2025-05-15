#include <stddef.h>
#include <stdlib.h>
#include "queue.h"
#include "private.h" 
#include "sem.h"


struct semaphore {
    size_t  count;     
    queue_t waiters;   
};

sem_t sem_create(size_t count)
{
    sem_t sem = (sem_t)malloc(sizeof(*sem));
    if (!sem)
        return NULL;

    sem->waiters = queue_create();
    if (!sem->waiters) {
        free(sem);
        return NULL;
    }

    sem->count = count;
    return sem;
}

int sem_destroy(sem_t sem)
{
    if (!sem)
        return -1;

    preempt_disable();


    if (queue_length(sem->waiters) != 0) {
        preempt_enable();
        return -1;
    }

    queue_destroy(sem->waiters);
    free(sem);

    preempt_enable();
    return 0;
}

int sem_down(sem_t sem)
{
    if (!sem)
        return -1;

    preempt_disable();

    while (sem->count == 0) {

        uthread_t self = uthread_current();
        queue_enqueue(sem->waiters, self);
        uthread_block();

        preempt_enable();
        preempt_disable();
    }


    sem->count -= 1;

    preempt_enable();
    return 0;
}

int sem_up(sem_t sem)
{
    if (!sem)
        return -1;

    preempt_disable();

    sem->count += 1;
    
    if (queue_length(sem->waiters) > 0) {
        uthread_t next;
        queue_dequeue(sem->waiters, (void **)&next);
        uthread_unblock(next);
    }

    preempt_enable();
    return 0;
}

