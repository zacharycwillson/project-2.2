#include <stddef.h>
#include <stdlib.h>
#include "queue.h"
#include "private.h"
#include "sem.h"

struct semaphore {
    int     value;
    queue_t waiting;
};

sem_t sem_create(size_t count)
{
    sem_t s = malloc(sizeof(*s));
    if (!s) return NULL;

    s->value   = (int)count;
    s->waiting = queue_create();
    if (!s->waiting) {
        free(s);
        return NULL;
    }
    return s;
}

int sem_destroy(sem_t sem)
{
    if (!sem) return -1;
    if (queue_length(sem->waiting) > 0)
        return -1;
    queue_destroy(sem->waiting);
    free(sem);
    return 0;
}

int sem_down(sem_t sem)
{
    if (!sem) return -1;
    preempt_disable();
    sem->value--;
    if (sem->value < 0) {
        struct uthread_tcb *me = uthread_current();
        queue_enqueue(sem->waiting, me);
        uthread_block();
    }

    preempt_enable();
    return 0;
}

int sem_up(sem_t sem)
{
    if (!sem) return -1;
    preempt_disable();
    sem->value++;
    if (sem->value <= 0) {
        void *td;
        queue_dequeue(sem->waiting, &td);
        uthread_unblock((struct uthread_tcb*)td);
    }

    preempt_enable();
    return 0;
}
