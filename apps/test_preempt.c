#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "uthread.h"

#define WORK_ITER 10000000

/* forward declarations */
static void thread_fn1(void *arg);
static void thread_fn2(void *arg);

int main(void) {
    /* start with thread_fn1; it will spawn thread_fn2 */
    (void)uthread_run(true, thread_fn1, NULL);
    putchar('\n');
    return 0;
}

static void thread_fn1(void *arg) {
    (void)arg;
    /* spawn the B‐printer under the new scheduler queue */
    if (uthread_create(thread_fn2, NULL) < 0) {
        perror("uthread_create");
        exit(1);
    }
    for (int i = 0; i < 20; i++) {
        putchar('A');
        fflush(stdout);
        for (volatile int j = 0; j < WORK_ITER; j++);
    }
    uthread_exit();
}

static void thread_fn2(void *arg) {
    (void)arg;
    for (int i = 0; i < 20; i++) {
        putchar('B');
        fflush(stdout);
        for (volatile int j = 0; j < WORK_ITER; j++);
    }
    uthread_exit();
}
