//  Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//  Defines
#define BUFFER 50

//  Structs
struct process_t{
    int id;
    char *name;
};

struct node {
    void *data;
    struct node *next;
};

struct queue_t {
    int count;
    struct node *first;
    struct node *last;
};
