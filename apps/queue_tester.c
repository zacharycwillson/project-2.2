#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

void test_length(void) 
{
	fprintf(stderr, "*** TEST length ***\n");
	//Create empty queue and test for length 0
	queue_t q = queue_create();
	TEST_ASSERT(queue_length(q) == 0);
	//Enqueue and validate length
	int a, b, c;
	queue_enqueue(q, &a);
	TEST_ASSERT(queue_length(q) == 1);
	queue_enqueue(q, &b);
	TEST_ASSERT(queue_length(q) == 2);
	queue_enqueue(q, &c);
	TEST_ASSERT(queue_length(q) == 3);
	//Dequeue and validate length
	void *tmp;
	queue_dequeue(q, &tmp);
	TEST_ASSERT(queue_length(q) == 2);
	queue_dequeue(q, &tmp);
	TEST_ASSERT(queue_length(q) == 1);
	queue_dequeue(q, &tmp);
	TEST_ASSERT(queue_length(q) == 0);

	queue_destroy(q);

}

void test_delete(void) 
{
	fprintf(stderr, "*** TEST delete ***\n");
	//Intitialize and enqueue
	int a = 1, b = 2, c = 3;
	queue_t q = queue_create();
	queue_enqueue(q, &a);
	queue_enqueue(q, &b);
	queue_enqueue(q, &c);
	//Test deleting b
	TEST_ASSERT(queue_delete(q, &b) == 0);
	TEST_ASSERT(queue_length(q) == 2);
	//Validate list is now A, Ca
	void *tmp;
	queue_dequeue(q, &tmp);
	TEST_ASSERT(tmp == &a);
	//Validate list is now C
	queue_dequeue(q, &tmp);
	TEST_ASSERT(tmp == &c);

	queue_destroy(q);

}

//Test iterate. Requires function to be defined onto the iterated data.
static int summed_result;

//Define summing function
void sum(queue_t q, void*data) 
{
	//Ignore queue pointer
	(void)q;
	summed_result += *(int*)data;
}

//Test iterate by summing through queue
void test_iterate(void) 
{
	fprintf(stderr, "*** TEST iterate ***\n");
	queue_t q = queue_create();
	int x = 4, y = 5, z = 6;
	queue_enqueue(q, &x);
	queue_enqueue(q, &y);
	queue_enqueue(q, &z);

	summed_result = 0;
	//Check that iterate exits process successfully
	TEST_ASSERT(queue_iterate(q, sum) == 0);
	//Validate iterate summing
	TEST_ASSERT(summed_result == (x + y + z));

	queue_destroy(q);
}

void test_destroy_nonempty(void) 
{
	fprintf(stderr, "*** TEST destroy_nonempty ***\n");
	int x = 30;
	queue_t q = queue_create();
	 queue_enqueue(q, &x);
	 //Validate destroy will return with error code
	 TEST_ASSERT(queue_destroy(q) == -1);

	 //Clean up memory
	 void *tmp;
	 queue_dequeue(q, &tmp);
	 TEST_ASSERT(queue_destroy(q) == 0);
}

void test_empty_operations() 
{
	fprintf(stderr, "*** TEST empty_operations ***\n");
	queue_t q = queue_create();
	void *tmp;
	TEST_ASSERT(queue_dequeue(q, &tmp) == -1);
	TEST_ASSERT(queue_delete(q, &tmp)    == -1);
	TEST_ASSERT(queue_destroy(q)         == 0);
}

void test_delete_head_tail_missing(void) 
{
	fprintf(stderr, "*** TEST delete_head_tail_missing ***\n");
	 int a = 1, b = 2, c = 3, d = 4;
	 queue_t q = queue_create();
	 queue_enqueue(q, &a);
	 queue_enqueue(q, &b);
	 queue_enqueue(q, &c);

	 //Delete head and validate new head is next node
	 TEST_ASSERT(queue_delete(q, &a) == 0);
	 void *tmp;
	 queue_dequeue(q, &tmp);
	 TEST_ASSERT(tmp == &b);

	 //Delete tail and validate new length
	 queue_enqueue(q, &c);
	 queue_enqueue(q, &d);
	 TEST_ASSERT(queue_delete(q, &d) == 0);
	 TEST_ASSERT(queue_length(q) == 2);

	 //Delete missing data value
	 int z = 50;
	 TEST_ASSERT(queue_delete(q, &z) == -1);

	 //Free memory
	 while(queue_dequeue(q, &tmp) == 0) {}
	 TEST_ASSERT(queue_destroy(q) == 0);

}


int main(void)
{
	//Validate function logic
	test_create();
	test_queue_simple();
	test_length();
	test_delete();
	test_iterate();
	//Validate error handling
	test_destroy_nonempty();
	test_empty_operations();
	test_delete_head_tail_missing();
	return 0;
}
