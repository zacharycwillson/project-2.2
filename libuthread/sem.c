#include <stddef.h>
#include <stdlib.h>
#include "queue.h"
#include "private.h"
#include "sem.h"

struct semaphore {
    int            value; 
    queue_t        waiting;
};

sem_t sem_create(size_t count)
{
    sem_t sem = malloc(sizeof(*sem));
    if (!sem) return NULL;

    sem->value   = (int)count;
    sem->waiting = queue_create();
    if (!sem->waiting) {
        free(sem);
        return NULL;
    }

    return sem;
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
        block(sem->waiting);
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
        unblock(sem->waiting);
    }
    preempt_enable();

    return 0;
}

