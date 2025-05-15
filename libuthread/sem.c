#include <stddef.h>
#include <stdlib.h>
#include "queue.h"
#include "private.h"
#include "sem.h"

struct semaphore {
    int     count;
    queue_t waiters;
};

sem_t sem_create(size_t count)
{
    sem_t s = malloc(sizeof(*s));
    if (!s) return NULL;

    s->count   = (int)count;
    s->waiters = queue_create();
    if (!s->waiters) {
        free(s);
        return NULL;
    }
    return s;
}

int sem_destroy(sem_t sem)
{
    if (!sem) return -1;
    if (queue_length(sem->waiters) > 0)
        return -1;

    queue_destroy(sem->waiters);
    free(sem);
    return 0;
}

int sem_down(sem_t sem)
{
    if (!sem) return -1;

    preempt_disable();
    if (sem->count > 0) {
        sem->count--;
    } else {
        struct uthread_tcb *me = uthread_current();
        queue_enqueue(sem->waiters, me);
        uthread_block();
    }
    preempt_enable();

    return 0;
}

int sem_up(sem_t sem)
{
    if (!sem) return -1;

    preempt_disable();
    if (queue_length(sem->waiters) > 0) {
        void *td;
        queue_dequeue(sem->waiters, &td);
        uthread_unblock((struct uthread_tcb*)td);
    } else {
        sem->count++;
    }
    preempt_enable();

    return 0;
}
