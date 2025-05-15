#define _POSIX_C_SOURCE 200809L  // for sigaction, setitimer
#include <stdio.h>  
#include <signal.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "private.h"
#include "uthread.h"

#define HZ 100

static struct sigaction old_action;
static struct itimerval   old_timer;
static bool                use_preempt = false;

static void preempt_handler(int signum)
{
    (void)signum;
    preempt_disable();
    uthread_yield();
    preempt_enable();
}

void preempt_disable(void)
{
    if (!use_preempt) return;

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void preempt_enable(void)
{
    if (!use_preempt) return;

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void preempt_start(bool preempt)
{
    use_preempt = preempt;
    if (!preempt) {
        return;
    }

    struct sigaction sa;
    sa.sa_handler = preempt_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGVTALRM, &sa, &old_action) < 0) {
        perror("sigaction");
        exit(1);
    }
    struct itimerval timer;
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_usec = 1000000 / HZ;
    timer.it_value = timer.it_interval; 
    if (setitimer(ITIMER_VIRTUAL, &timer, &old_timer) < 0) {
        perror("setitimer");
        exit(1);
    }
}

void preempt_stop(void)
{
    if (!use_preempt) return;
    if (setitimer(ITIMER_VIRTUAL, &old_timer, NULL) < 0) {
        perror("setitimer restore");
        exit(1);
    }
    if (sigaction(SIGVTALRM, &old_action, NULL) < 0) {
        perror("sigaction restore");
        exit(1);
    }
}

