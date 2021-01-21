/*
Project 0: Getting Started 
    Part 4 Linked Queue
Author: Anna DeVries
Date: 21 September 2020

Useage: ./queue

Queue is described as (ID, name) groupings

Editted from original project to include currernt project requirements - 8 November 2020
*/

//  Includes
#include "queue.h"

//  Global variables
typedef struct node node;
typedef struct queue_t queue_t;
typedef struct process_c process_c;
typedef struct process_i process_i;
typedef struct process_t process_t;

//  Enqueue() that adds elements to the end of the queue
void enqueue(queue_t *q, void *element){
    //  Local variables
    struct node *add_element = malloc(sizeof(struct node)); 
    add_element->data = element;
    add_element->next = NULL;

    //  Check if queue has data 
    if(q->last == NULL){
        q->first = q->last = add_element;
    }

    //  Else add element to queue
    else{
        q->last->next = add_element;
        q->last = add_element;
    }

    //  Increase counter in queue
    q->count = q->count + 1;

    //  Return function
    return;
}

//  Dequeue() that removes and returns the element in the front of the queue
void *dequeue(queue_t *q){
    //  Local variables
    struct node *rm_element = malloc(sizeof(struct node)); 
    void *data;

    //  Check if queue is empty
    if(q == NULL || q->count == 0){
        //  Clean rm_element
        free(rm_element);
        return 0;
    }

    //  Add null to next value
    if( q->count == 1){
        q->first->next = NULL;
        q->last = NULL;
    }

    //  Initialize variables
    data = q->first->data;
    rm_element = q->first;

    //  Shift linked list by one
    q->first = q->first->next;

    //  Check if queue is null
    if (q->first != NULL){
      q->first->next = NULL;
    }

    //  Decrement size of queue
    q->count = q->count - 1;

    //  Clean rm_element
    free(rm_element);

    //  Return popped value
    return data;
}

//  Print queue
void printing(node *top){
    //  Check for empty or null queue
    if (top == NULL){
        printf("\n\n");

        //  Return function
        return;
    }
    else{
      //  Print queue element
      printf("%s", (char*) top->data);

      //  Repeat for each element in list
      printing(top->next);
    }

    //  Return function
    return;
}

//  Print queue with ints
void printing_int(node *top){
    //  Check for empty or null queue
    if (top == NULL){
        printf("\n");

        //  Return function
        return;
    }
    else{
      //  Print queue element
      printf("%ld ", (uintptr_t) (int *) top->data);

      //  Repeat for each element in list
      printing_int(top->next);
    }

    //  Return function
    return;
}

//  Test Case function
void testing(struct queue_t *q, struct process_c *p){
    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first);

    //  Process_c Set One
    p->character = "a";
    enqueue(q, p->character);
    p->character = "c";
    enqueue(q, p->character);
    p->character = " ";
    enqueue(q, p->character);
    p->character = "abcdef";
    enqueue(q, p->character);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first);

    //  Delete process_t
    printf("\tpop value from queue.\n");

    //  Add process to queue
    dequeue(q);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first);

    //  Return function
    return;
}
