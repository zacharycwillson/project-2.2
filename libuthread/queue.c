#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>  
#include "queue.h"

/*Implemented by Zachary Wiilson*/
/*FIFO queue is implemented via a singly linked list composed of two respective structs, achieving O(1) for the specified functions*/


//Linked list node for data and next node pointers
struct node 
{
	void *data;
	struct node *next;
};


//FIFO queue struct with head and tail pointers and size value
struct queue 
{
	struct node *head;
	struct node *tail;
	size_t size;
	
};

queue_t queue_create(void)
{
	//Malloc Memory for queue
	queue_t new_queue = malloc(sizeof(struct queue));
	//return Null if malloc failed
	if (!new_queue) 
	{
		return NULL;
	}
	//Initialize struct values to empty/0
	new_queue->head = NULL;
	new_queue->tail = NULL;
	new_queue->size = 0;

	return new_queue;
}

int queue_destroy(queue_t queue)
{
	//Check that queue is non-empty
	if (queue == NULL || queue->size != 0) 
	{
		return -1;
	}

	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	//Validate queue parameter
	if(!queue || !data) 
	{
		return -1;
	}
	//malloc memory for new node
	struct node *new_node = malloc(sizeof(struct node));
	//If malloc fails return error
	if (!new_node) 
	{
		return -1;
	}
	//Set new_node values
	new_node->data = data;
	new_node->next = NULL;
	//If queue is empty, new_node is both head and tail
	if (queue->head == NULL) 
	{
		queue->head = new_node;
		queue->tail = new_node;
	}
	else 
	{
		//List is non-empty. Set tail to point to new_node and set new_node as tail
		queue->tail->next = new_node;
		queue->tail = new_node;
	}

	queue->size += 1;
	return 0;

}

int queue_dequeue(queue_t queue, void **data)
{
	//validate queue and data
	if (!queue || !data || queue->head == NULL) 
	{
		return -1;
	}

	//update data parameter
	*data = queue->head->data;
	//create old_node so that head may be updated and dequeue node may be freed
	struct node *old_head = queue->head;
	queue->head = old_head->next;
	//If head is NULL, list is empty so set tail to NULL
	if (queue->head == NULL) 
	{
		queue->tail = NULL;
	}
	//Free old_head and update queue size
	free(old_head);
	queue->size -= 1;
	return 0;


	
}

int queue_delete(queue_t queue, void *data)
{
	//validate arguments
	if (!queue) 
	{
		return -1;
	}

	//Start list search at head of queue
	struct node *previous = NULL;
	struct node *current = queue->head;

	while(current) 
	{
		if (current->data == data) 
		{
			//Data is found, update nodes and free current;
			if (previous == NULL) 
			{
				queue->head = current->next;
			}
			else
			{
				previous->next = current->next;
			}
			if (queue->tail == current)
			{
				queue->tail = previous;
			}
			free(current);
			queue->size -= 1;
			return 0;

		}
		else 
		{
			//Data was not found, update nodes for next search iteration
			previous = current;
			current = current->next;
		}
	}
	//Data was not found, return error
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	//Validate parameters
	if (!queue || !func) 
	{
		return -1;
	}
	//Step through list starting at head and pass data values
	struct node *current = queue->head;
	while (current) 
	{
		struct node *next_node = current->next;
		func(queue, current->data);
		current = next_node;
	}
	return 0;
}

int queue_length(queue_t queue)
{
	//If NULL return error, otherise return size of queue
	if (queue == NULL) 
	{
		return -1;
	}

	return(int)queue->size;
}

