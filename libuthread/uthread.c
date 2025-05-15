#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

//Define global varaibles
extern queue_t         scheduler_queue;
extern struct uthread_tcb *current_thread;
static int next_thread_ID = 0; //Assign thread ID values in incrementing order

//Define all possible thread states for uthread_state_t enumeration
typedef enum 
{
	RUNNING,
	READY, 
	BLOCKED, 
	ZOMBIE, 
} uthread_state_t;

//Define thread control block struct to store thread properties
struct uthread_tcb 
{
	uthread_ctx_t context; // Save CPU registers & instruction pointer for thread
	uthread_state_t state; // Current thread scheduler state as enumerated above
	void*stack; //Pointer to top of thread’s allocated stack region
	int thread_ID; //Thread identifer for debugging

};

//Return tcb pointer of currently running thread
struct uthread_tcb *uthread_current(void)
{
	/*TODO PHASE 3*/
	return current_thread;
}

void uthread_yield(void)
{
	//Yield current thread by switching it from running to ready and reschedule it to the back of the queue
	current_thread->state = READY;
	queue_enqueue(scheduler_queue, current_thread);
	//Dequeue next thread from scheduler and save it as next_thread
	struct uthread_tcb *next_thread;
	int dequeue_error = queue_dequeue(scheduler_queue, (void**)&next_thread);
	if (dequeue_error == -1) //error if queue is empty - nothing to dequeue
	{
		return;
	}
	//Set next_thread as running and pass it to current_thread
	next_thread->state = RUNNING;
	struct uthread_tcb *previous_thread = current_thread;
	current_thread = next_thread;
	uthread_ctx_switch(&previous_thread->context, &current_thread->context);

}

void uthread_exit(void)
{
	//Mark thread as dead
	current_thread->state = ZOMBIE;
	//Free thread's stack
	uthread_ctx_destroy_stack(current_thread->stack);
	//Dequeue next ready thread
	struct uthread_tcb *next_thread;
	int dequeue_error = queue_dequeue(scheduler_queue, (void**)&next_thread);
	//Error handle
	if (dequeue_error == -1) 
	{
		preempt_stop();
		exit(0);
	}
	//Switch to next thread context
	next_thread->state = RUNNING;
	struct uthread_tcb *dead_thread = current_thread;
	current_thread = next_thread;
	uthread_ctx_switch(&dead_thread->context, &next_thread->context);
}

int uthread_create(uthread_func_t func, void *arg)
{
	//Allocate memory for thread control block
	struct uthread_tcb *tcb = malloc(sizeof *tcb);
	//Validate control block initialization
	if (!tcb) 
	{
		return -1;
	}
	//Allocate thread control block stack
	tcb->stack = uthread_ctx_alloc_stack();
	//Validate stack allocation
	if (!tcb->stack) 
	{
		free(tcb);
		return -1;
	}
	//Initalize thread control block context and validate
	if (uthread_ctx_init(&tcb->context, tcb->stack, func, arg)) 
	{
		uthread_ctx_destroy_stack(tcb->stack);
		free(tcb);
		return -1;
	}
	//Assign ID and state
	tcb->thread_ID = next_thread_ID++;
	tcb->state     = READY;
	//Enqueue thread control block on the scheduler queue
	queue_enqueue(scheduler_queue, tcb);
	return 0;


}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	//TODO: PREEMPT PHASE 4
	preempt_start(preempt);

	//Initialize scheduler queueue and validate
	scheduler_queue = queue_create();
	if (!scheduler_queue) 
	{
		return -1;
	}
	//Create idle context for thread bootstrapping
	struct uthread_tcb idle;
	getcontext(&idle.context);
	idle.state = RUNNING;
	current_thread = &idle;
	//Bootstrap first thread and validate
	int create_error = uthread_create(func, arg);
	if (create_error == -1) 
	{
		return -1;
	}
	//Enter main running loop: Process threads round-robin until all zombies are terminated
	while (queue_length(scheduler_queue) > 0) 
	{
		//Dequeue next ready thread and validate
		struct uthread_tcb *next_thread;
		int dequeue_error = queue_dequeue(scheduler_queue, (void**)&next_thread);
		if (dequeue_error == -1) 
		{
			return -1;
		}
		//Check if current thread is zombie and terminate if so
		if (current_thread->state == ZOMBIE && current_thread != &idle) 
		{
			free(current_thread);
		}
		//Switch to next thread's context
		struct uthread_tcb *previous_thread = current_thread;
		next_thread->state = RUNNING;
		current_thread = next_thread;
		uthread_ctx_switch(&previous_thread->context, &next_thread->context);


	}

	//TODO: PREEMPT PHASE 4
	preempt_stop();
	return 0;
}

void uthread_block(void)
{
    struct uthread_tcb *me   = uthread_current();
    struct uthread_tcb *next = NULL;

    me->state = BLOCKED;
    if (queue_dequeue(scheduler_queue, (void**)&next) < 0) {
        exit(1);
    }
    next->state     = RUNNING;
    current_thread  = next;
    uthread_ctx_switch(&me->context, &next->context);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
    if (uthread->state == BLOCKED) {
        uthread->state = READY;
        queue_enqueue(scheduler_queue, uthread);
    }
}
