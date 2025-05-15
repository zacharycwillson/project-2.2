#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "private.h"
#include "sem.h"

struct semaphore {
    size_t          count; 
    queue_t         waiters;
};

#define RETURN_IF(cond, val) do { if (cond) { return (val); } } while (0)

sem_t sem_create(size_t count)
{
    struct semaphore *sem = malloc(sizeof(*sem));
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
    RETURN_IF(!sem, -1);

    if (!queue_isempty(sem->waiters))
        return -1;

    queue_destroy(sem->waiters);
    free(sem);
    return 0;
}

int sem_down(sem_t sem)
{
    RETURN_IF(!sem, -1);

    preempt_disable();

    while (sem->count == 0) {

        queue_enqueue(sem->waiters, uthread_current());
        uthread_block();
    }

    sem->count--;

    preempt_enable();
    return 0;
}

int sem_up(sem_t sem)
{
    RETURN_IF(!sem, -1);

    preempt_disable();

    if (!queue_isempty(sem->waiters)) {
        struct uthread_tcb *tcb;
        queue_dequeue(sem->waiters, (void**)&tcb);
        uthread_unblock(tcb);
    } else {
        sem->count++;
    }

    preempt_enable();
    return 0;
}

