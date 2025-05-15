#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "uthread.h"

#define WORK_ITER 10000000

static void thread_fn1(void *arg) {
    for (int i = 0; i < 20; i++) {
        putchar('A');
        fflush(stdout);
        for (volatile int j = 0; j < WORK_ITER; j++);
    }
}

static void thread_fn2(void *arg) {
    for (int i = 0; i < 20; i++) {
        putchar('B');
        fflush(stdout);
        for (volatile int j = 0; j < WORK_ITER; j++);
    }
}

int main(void) {
    if (uthread_create(thread_fn1, NULL) < 0) {
        perror("uthread_create");
        exit(1);
    }
    if (uthread_create(thread_fn2, NULL) < 0) {
        perror("uthread_create");
        exit(1);
    }
    uthread_run(true);

    putchar('\n');
    return 0;
}
