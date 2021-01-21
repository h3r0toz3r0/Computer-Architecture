/*
Project 0: Getting Started 
    Part 4 Linked Queue
Author: Anna DeVries
Date: 21 September 2020

Useage: ./queue

Queue is described as (ID, name) groupings
*/

//  Includes
#include "queue.h"

//  Global variables
typedef struct node node;
typedef struct queue_t queue_t;
typedef struct process_t process_t;

//  Enqueue() that adds elements to the end of the queue
void enqueue(queue_t *q, void *element){
    //  Local variables
    struct node *add_element = malloc(sizeof(struct node)); 
    add_element->data = element;
    add_element->next = NULL;

    //  Check is queue is full
    if (q->count == BUFFER){
        printf("Queue buffer reached.\n");
    }
    else{
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
    }

    //  Return function
    return;
}

//  Dequeue() that removes and returns the element in the front of the queue
void *dequeue(queue_t *q){
    //  Local variables
    struct node *rm_element = malloc(sizeof(struct node)); 
    void *data;

    //  Initialize variables
    data = q->first->data;
    rm_element = q->first;
    
    //  Shift linked list by one
    q->first = q->first->next;

    //  Decrement size of queue
    q->count = q->count - 1;

    //  Clean rm_element
    free(rm_element);

    //  Return popped value
    return data;
}

//  Print queue
void printing(node *top, int i){
    //  Check for empty or null queue
    if (top == NULL){
        printf("\n\n");

        //  Return function
        return;
    }
    else{
        if (i == 0){
            //  Print queue element
            printf("\t\t(%ld,", (uintptr_t) (int *) top->data);

            //  Repeat for each element in the list
            printing(top->next, 1);
        }
        else{
            //  Print queue element
            char* string_val = (char*) top->data;
            printf(" %s) ", string_val);

            //  Repeat for each element in the list
            printing(top->next, 0);
        }
    }

    //  Return function
    return;
}

//  Test Case function
void testing(struct queue_t *q, struct process_t *p){
    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first, 0);

    //  Process_t Set One
    p->id = 1;
    p->name = "Alpha";
    printf("\tadded process_t Set One: %d %s\n", p->id, p->name);

    //  Add process to queue
    enqueue(q, (void *) (intptr_t) p->id);
    enqueue(q, p->name);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first, 0);

    //  Process_t Set Two
    p->id = 2;
    p->name = "Bravo";
    printf("\tadded process_t Set Two: %d %s\n", p->id, p->name);

    //  Add process to queue
    enqueue(q, (void *) (intptr_t) p->id);
    enqueue(q, p->name);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first, 0);

    //  Process_t Set Three
    p->id = 3;
    p->name = "Charlie";
    printf("\tadded process_t Set Three: %d %s\n", p->id, p->name);

    //  Add process to queue
    enqueue(q, (void *) (intptr_t) p->id);
    enqueue(q, p->name);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first, 0);

    //  Delete process_t
    printf("\tpop value from queue.\n");

    //  Add process to queue
    dequeue(q);
    dequeue(q);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first, 0);

    //  Process_t Set Four
    p->id = 4;
    p->name = "Delta";
    printf("\tadded process_t Set Four: %d %s\n", p->id, p->name);

    //  Add process to queue
    enqueue(q, (void *) (intptr_t) p->id);
    enqueue(q, p->name);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first, 0);

    //  Process_t Set Five
    p->id = 5;
    p->name = "Echo";
    printf("\tadded process_t Set Five: %d %s\n", p->id, p->name);

    //  Add process to queue
    enqueue(q, (void *) (intptr_t) p->id);
    enqueue(q, p->name);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first, 0);

    //  Process_t Set Six
    p->id = 6;
    p->name = "Foxtrot";
    printf("\tadded process_t Set Six: %d %s\n", p->id, p->name);

    //  Add process to queue
    enqueue(q, (void *) (intptr_t) p->id);
    enqueue(q, p->name);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first, 0);

    //  Delete process_t
    printf("\tpop value from queue.\n");

    //  Add process to queue
    dequeue(q);
    dequeue(q);

    //  Prints current Queue state
    printf("\t\tCurrent Queue: ");
    printing(q->first, 0);

    //  Return function
    return;
}

//  Main function
int main(int argc, char* argv[]){
    //  Local variables
    struct queue_t *q = malloc(sizeof(queue_t));
    struct process_t *p = malloc(sizeof(process_t));

    //  Initailize queue to nothing
    q->count = 0;
    q->first = NULL;
    q->last = NULL;

    //  Intro print statements
    printf("\nSimple Queue Program.\nOrganized as (ID, NAME) left to right.\n");
    printf("Left being the oldest entries and right being the newest entries.\n\n");

    //  Test Cases
    testing(q, p);

    //  Return and exit
    return 0;
    
}

