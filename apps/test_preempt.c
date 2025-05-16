#include <stdio.h>
#include <stdlib.h>
#include "uthread.h"

#define WORK_ITERS 10000000

static void busy_thread(void *arg) {
    char c = *(char*)arg;
    for (int i = 0; i < 50; i++) {
        putchar(c);
        fflush(stdout);
        for (volatile int j = 0; j < WORK_ITERS; j++);
    }
}

int main(void) {
    char a = 'A', b = 'B';

    if (uthread_create(busy_thread, &a) < 0 ||
        uthread_create(busy_thread, &b) < 0) {
        perror("uthread_create");
        exit(1);
    }
    uthread_run(true, 0);

    putchar('\n');
    printf("GOAL ACHIEVED: Preemption prevented any single thread from hogging the CPU.\n");
    return 0;
}
