/*
    Programing Assignment #2
    Date: 2 November

    Authors: Anna DeVries and Vaughn Franz

    Database Server
*/

/*  Include */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "proj2.h"
#include "queue.c"
#include <string.h>
#include <errno.h>

extern int errno;

/*  Define  */
#define SIZE 200
#define REQ_SIZE 40
#define REQ_NAME_SIZE 31
#define REQ_NAME_LEN 8
#define DEBUG 0
/*
Debug = 1 for printing basic text, stats, errors, sending/receiving requests, and work queue info
Debug = 2 for printing basic text, stats, errors and sending/receiving requests
Debug = 3 for printing basic text, stats and errors
Debug = 0 only prints stats
*/

/*  Mutexes     */
pthread_mutex_t num_read_requests_lock;
pthread_mutex_t num_write_requests_lock;
pthread_mutex_t num_delete_requests_lock;
pthread_mutex_t num_failed_requests_lock;
pthread_mutex_t queued_requests_lock;
pthread_mutex_t queue_lock;
pthread_mutex_t table_lock;

/*  Structs         */
//  Holds a struct called table entry for holding a string of the name and valid bit
typedef struct table_entry{
    //  File name variable
    char* name;
    //  Valid = v   Invalid = i Busy = b
    char valid_bit;
} table_entry;

//  Holds a struct to pass request, length and buffer between functions
typedef struct read_struct{
    //  Length of buffer
    int len;
    //  Buffer to return to socket
    char *buf;
} read_struct;


/*  Macro Values    */
struct queue_t *q;
struct process_c *p;
struct process_i *p_i;
table_entry** table;
int num_read_requests = 0, num_write_requests = 0, num_delete_requests = 0;
int num_failed_requests = 0, queued_requests = 0;
int checker = 1;

/*  Helper functions    */
/*  Stats command   */
void stats_func(){
    //  Print the number of objects in table
    int counter = 0;
    pthread_mutex_lock(&table_lock);
    for(int i = 0; i < SIZE; i++){
        if(table[i]->valid_bit == 'v'){
            counter++;
        }
    }
    pthread_mutex_unlock(&table_lock);
    printf("Number of objects in the table: %d\n", counter);

    //  Print number of read, write and delete requests received
    printf("Total read requests:\t\t%d\n", num_read_requests);
    printf("Total write requests:\t\t%d\n", num_write_requests);
    printf("Total delete requests:\t\t%d\n", num_delete_requests);

    //  Print the number of requests queued waiting for worker threads
    printf("Queued requests:\t\t%d\n", queued_requests);

    //  Print number of failed requests
    printf("Failed requests:\t\t%d\n", num_failed_requests);

    printf("---------------------------------------------\n\n");

    return;
}

//  Linear search global array table for free entry
int find_free_entry(){
    //  Iterate entries for invalid
    pthread_mutex_lock(&table_lock);
    for( int i = 0; i < SIZE; i++){
        if(table[i]->valid_bit == 'i'){
            //  Returns index of first invalid entry
            return i;
        }
    }
    pthread_mutex_unlock(&table_lock);
    //  No free entries; return -1 for failure
    return -1;
}

//  Linear search for database entry
int find_entry(char *name){
    //  Iterate through table looking for specific name
    pthread_mutex_lock(&table_lock);
    for( int i = 0; i < SIZE; i++){
        if( strcmp(table[i]->name, name) == 0){
            //  Return index with the name
            return i;
        }
    }
    pthread_mutex_unlock(&table_lock);
    //  Entry not found; returns -1 for failure
    return -1;
}

//  Write byte by byte to file
int write_to_file(node *top, FILE *fp){
    //  Local variables
    int error = 0;

    //  Check for empty or null queue
    if (top == NULL){
        //  Return function
        return 0;
    }
    else{
        //  Turn data into string
        char* data = (char*) top->data;

        //  Write to file
        error = fputs(data, fp);

        //  Check for error
        if( error < 0){
            return -1;
        }

        //  Repeat for each element in queue
        write_to_file(top->next, fp);
    }

    //  return function
    return 0;
}

//  Write request
void write_request(struct request* rq_rcv, struct request* rq_snd, char buf[]){
    //  Local variables
    int index;

    //  Add name and length into send request
    strcpy(rq_snd->name, rq_rcv->name);
    strcpy(rq_snd->len, "0000000");
    rq_snd->op_status = 'X';

    //  Look for prior existence of name
    index = find_entry(rq_snd->name);

    //  Find first available entry in table if no prior existence
    if( index < 0){
        index = find_free_entry();
    }
    //  Send back failed if rewrite on busy state
    else{
        if (table[index]->valid_bit == 'b'){
            rq_snd->op_status = 'X';
            pthread_mutex_unlock(&table_lock);
            return;
        }
        table[index]->valid_bit = 'b';
        pthread_mutex_unlock(&table_lock);
    }

    //  If no available entries, update status and quit
    if ( index < 0){
        if(DEBUG > 0){
            printf("\tNo free space for write request.\n");
        }
        rq_snd->op_status = 'X';

        //  Return function
        return;
    }
    else{
        //  Update table to denote being busy
        strcpy(table[index]->name, rq_rcv->name);
        table[index]->valid_bit = 'b';
        pthread_mutex_unlock(&table_lock);

        //  Add sleep to check for race conditions
        usleep(random() % 10000);

        //  Create the file based on next available file name
        FILE *fp;
        char file_name[13];  //  can hold either data.0 or data.199

        //  Concatentate filename with empty table entry index
        sprintf(file_name, "/tmp/data.%d", index);

        if(DEBUG == 1){
            printf("\tWriting to... %s\n", file_name);
        }

        //  Open file for writing strings
        fp = fopen (file_name, "w+");

        //  Unable to open file
        if(fp == NULL){
            if(DEBUG > 0){
                printf("\tfailed to open file\n");
            }

            fclose(fp);
            return;
        }

        //  Place buffer into file
        int error = fputs(buf, fp);

        //  Error placing buffer into file
        if(error < 0){
          rq_snd->op_status = 'X';
          pthread_mutex_lock(&table_lock);
          table[index]->valid_bit = 'i';
          pthread_mutex_unlock(&table_lock);
        }

        //  Success placing buffer into file
        else {
          rq_snd->op_status = 'K';
          pthread_mutex_lock(&table_lock);
          table[index]->valid_bit = 'v';
          pthread_mutex_unlock(&table_lock);
        }

        //  Close file
        fclose(fp);
    }

    //  Return function
    return;
}

//  Read request
read_struct* read_request(struct request* rq_rcv, struct request* rq_snd){
    //  Local variables
    read_struct* read_strct = malloc(sizeof(read_struct));
    read_strct->buf = malloc(sizeof(char) * 4096);
    int index;

    //  Add name, len, status failed by default
    strcpy(rq_snd->name, rq_rcv->name);
    strcpy(rq_snd->len, rq_rcv->len);
    rq_snd->op_status = 'X';

    //  Search for name
    index = find_entry(rq_rcv->name);

    //  Value doesnt exist in table
    if ( index < 0){
        if(DEBUG > 0){
            printf("\tNo table entry found.\n");
        }
        rq_snd->op_status = 'X';
        //  Return function
        return read_strct;
    }

    else{
        //  Check if valid bit is busy, return if so
        if(table[index]->valid_bit == 'b' || table[index]->valid_bit == 'i'){
            rq_snd->op_status = 'X';
            if(DEBUG > 0){
                printf("busy file can't read\n");
            }
            //  Return function
            pthread_mutex_unlock(&table_lock);
            return read_strct;
        }
        table[index]->valid_bit = 'b';
        pthread_mutex_unlock(&table_lock);

        //  Create the file based on next available file name
        FILE *fp;
        char file_name[13];  //  can hold either data.0 or data.199

        //  Concatentate filename with empty table entry index
        sprintf(file_name, "/tmp/data.%d", index);

        //  Open file for reading strings
        fp = fopen (file_name, "r");

        if(fp == NULL){
            if(DEBUG > 0){
                fprintf(stderr, "Error opening the file: %s\n", strerror( errno ));
            }
            pthread_mutex_lock(&table_lock);
            table[index]->valid_bit = 'i';
            pthread_mutex_unlock(&table_lock);
            return read_strct;
        }
        //  Read every character in file
        int counter = 0;
        while (1) {
            //  get character
            int c = fgetc(fp);

            //  Exit at end of file
            if (feof(fp) || counter > 4096){
                fclose(fp);
                break;
            }

            //  Place character into buffer
            read_strct->buf[counter] = (char) c;
            counter++;

        }

        //  Update op status and valid bit in table
        rq_snd->op_status = 'K';
        read_strct->len = counter;
        pthread_mutex_lock(&table_lock);
        table[index]->valid_bit='v';
        pthread_mutex_unlock(&table_lock);

        //  Convert length to string
        if( counter < 10 && counter >= 1){
            sprintf(rq_snd->len, "000000%d", counter);
        }
        else if(counter < 100){
            sprintf(rq_snd->len, "00000%d", counter);
        }
        else if(counter < 1000){
            sprintf(rq_snd->len, "0000%d", counter);
        }
        else if(counter < 10000){
            sprintf(rq_snd->len, "000%d", counter);
        }
        else if(counter == 0){
            sprintf(rq_snd->len, "000000%d", counter);
        }
    }

    //  Return function
    return read_strct;
}

//  Delete request
void delete_request(struct request* rq_rcv, struct request* rq_snd){
    //  Add name and length into send request
    strcpy(rq_snd->name, rq_rcv->name);
    strcpy(rq_snd->len, "0000000");
    rq_snd->op_status = 'X';

    //  Search table for index
    int index = find_entry(rq_rcv->name);

    //  Value doesnt exist in table
    if ( index < 0){
        if(DEBUG > 0){
            printf("\tNo table entry found.\n");
        }
        rq_snd->op_status = 'X';

        //  Return function
        return;
    }

    else{
        //  Check if valid bit is busy, return if so
        if(table[index]->valid_bit == 'b'){
            rq_snd->op_status = 'X';

            //  Return function
            pthread_mutex_unlock(&table_lock);
            return;
        }
        table[index]->valid_bit='b';
        pthread_mutex_unlock(&table_lock);
    }

    //  Create filename to delete
    char file_name[13];  //  can hold either data.0 or data.199

    //  Concatentate filename with empty table entry index
    sprintf(file_name, "/tmp/data.%d", index);

    //  Remove file
    int error = remove(file_name);

    //  Ensure file removed successful
    if (error == 0){
        //  If successful, update op status and table entry to be empty
        rq_snd->op_status = 'K';
        pthread_mutex_lock(&table_lock);
        table[index]->valid_bit = 'i';
        pthread_mutex_unlock(&table_lock);
    }
    else{
        rq_snd->op_status = 'X';
    }

    //  Return function
    return;
}


//  Reads a request from a TCP connection and performs the requested action (write/read/delete)
int handle_work(int sock_fd){
    //  Local variables
    int error;
    struct request *rq_rcv = malloc(sizeof(struct request));
    struct request *rq_snd = malloc(sizeof(struct request));
    read_struct *read_strct = malloc(sizeof(read_struct));

    //  First read into request
    error = read(sock_fd, rq_rcv, sizeof(struct request));

    //  Handle error in read()
    if( error < 0){
        if(DEBUG > 0){
            printf("\tError in read request.\n");
        }
        return 0;
    }

    //  Convert length into int
    int len = atoi(rq_rcv->len);

    //  Receive data into buffer if write
    char buffer[4096];
    if(rq_rcv->op_status == 'W'){
      error = read(sock_fd, buffer, len);
    }
    buffer[len] = '\0';

    //  Handle error in read()
    if( error < 0){
        if(DEBUG > 0){
            printf("\tError in read request.\n");
        }
        return 0;
    }

    //  Add name, op and length
    rq_snd->op_status = 'X';
    strcpy(rq_snd->name, rq_rcv->name);
    strcpy(rq_snd->len, rq_rcv->len);

    //  Logical operations
    //  Write
    if( rq_rcv->op_status == 'W'){
        //  Enter write function
        write_request(rq_rcv, rq_snd, buffer);
    }
    //  Read
    else if( rq_rcv->op_status == 'R'){
        //  Enter write function
        read_strct = read_request(rq_rcv, rq_snd);
    }
    //  Delete
    else if( rq_rcv->op_status == 'D'){
        //  Enter delete function
        delete_request(rq_rcv, rq_snd);
    }

    //  Print sending request
    if(DEBUG == 2 || DEBUG == 1){
        printf("---------------------------------------------\n");
        printf("New Request\n");
        printf("Received Request:\tOp: %c\tName: %s\tlen: %s\n", rq_rcv->op_status, rq_rcv->name, rq_rcv->len);
        if( rq_rcv->op_status == 'W'){
            printf("\tWriting Buffer: %s\n", buffer);
        }
        printf("Sending Request:\tOp: %c\tName: %s\tLen: %s\n", rq_snd->op_status, rq_snd->name, rq_snd->len);
        if( rq_rcv->op_status == 'R'){
            printf("Buffer: %s\n", read_strct->buf);
        }
        printf("---------------------------------------------\n\n");
    }

    //  Write request to socket
    error = write(sock_fd, rq_snd, sizeof(struct request));

    //  Handle error in write()
    if( error < 0){
        if(DEBUG > 0){
            printf("\tError in write request.\n");
        }
    }

    //  If read request, also include the buffer when writing to socket
    if ( rq_rcv->op_status == 'R' ){ 
        error = write(sock_fd, read_strct->buf, read_strct->len);

        //  Handle error in write()
        if( error < 0){
            if(DEBUG > 0){
                printf("\t READ RESPONSE: Error in write request.\n");
            }
        }
    }

    //  Close socket
    close(sock_fd);

    //  Add request to counter
    if(rq_rcv->op_status == 'R'){
        pthread_mutex_lock(&num_read_requests_lock);
        num_read_requests++;
        pthread_mutex_unlock(&num_read_requests_lock);
        free(read_strct->buf);
    }
    else if(rq_rcv->op_status == 'W'){
        pthread_mutex_lock(&num_write_requests_lock);
        num_write_requests++;
        pthread_mutex_unlock(&num_write_requests_lock);
    }
    else if(rq_rcv->op_status == 'D'){
        pthread_mutex_lock(&num_delete_requests_lock);
        num_delete_requests++;
        pthread_mutex_unlock(&num_delete_requests_lock);
    }
    if(rq_snd->op_status == 'X'){
        pthread_mutex_lock(&num_failed_requests_lock);
        num_failed_requests++;
        pthread_mutex_unlock(&num_failed_requests_lock);
    }

    //  Free memory
    free(rq_rcv);
    free(rq_snd);
    free(read_strct);

    //  return function
    return 0;
}

//  Allocates a work item and puts it on a queue
void queue_work(int sock_fd){
    //  Create p_i process
    p_i = malloc(sizeof(process_t));

    //  Add p_i->fd to queue
    p_i->fd = sock_fd;
    enqueue(q, (void *) (intptr_t) p_i->fd);

    //  return functions
    return;
}

//  Gets an item from the queue and frees the work record
int get_work(void){
    //  Set fd defialt to be -1 for no nodes
    int fd = -1;

    //  Check for an empty queue
    if (q->count == 0){
        return fd;
    }
    //  Check for one element in queue
    else if(q->count == 1){
        //  Pull data, convert to int
        fd = (uintptr_t) (int *) q->first->data;

        //  Update the queue to be empty
        q->first = NULL;
        q->last = NULL;
        q->count = 0;

        //  Return fd
        return fd;
    }

    //  Pull data, convert to int
    fd = (uintptr_t) (int *) q->first->data;

    //  Update queue to reflect changes
    struct node *rm_element = malloc(sizeof(struct node));
    rm_element = q->first;
    q->first = q->first->next;
    q->count--;

    //  free rm element
    free(rm_element);

    //  return sock_fd
    return fd;
}

//  worker thread function
void *worker_thread_func(){
    //  Four worker threads
    while ( 1 ){
        //  Grab first available work value
        pthread_mutex_lock(&queue_lock);
        int sock_fd = get_work();
        if(DEBUG == 1 && sock_fd > 0){
            printf("Updated queue: ");
            printing_int(q->first);
            printf("Popped... %d\n", sock_fd);
            printf("\n");
        }
        pthread_mutex_unlock(&queue_lock);

        //  Subtract from queue global variable
        if(sock_fd > 0){
            pthread_mutex_lock(&queued_requests_lock);
            queued_requests--;
            pthread_mutex_unlock(&queued_requests_lock);

            //  Get work id and handle work
            if(handle_work(sock_fd) < 0){
                exit(0);
            }
        }
    }

    //  Exit the pthread and return function
    pthread_exit(NULL);
    return NULL;
}

//  Creates and binds the listening socket; loops accepting connections and calling
void *listener(void *port_void_ptr){
    //  Cast port back to int
    int *port_ptr = (int *) port_void_ptr;
    int port = *port_ptr;

    //  Create a listening TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    //  The address we'll bind it to
    struct sockaddr_in addr = {.sin_family = AF_INET,
                                .sin_port = htons(port),
                                .sin_addr.s_addr = 0};

    //  Bind the listening socket to the port
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        perror("can't bind"), exit(1);

    //  Tell the OS to start listening on it.
    if (listen(sock, 2) < 0)
        perror("listen"), exit(1);

    //  Continuously accept connections
    while ( 1 ){
        //  Accept connection
        int fd = accept(sock, NULL, NULL);

        //  Add to global variable
        if(fd > 0){
            pthread_mutex_lock(&queued_requests_lock);
            queued_requests++;
            pthread_mutex_unlock(&queued_requests_lock);
            pthread_mutex_lock(&queue_lock);
            queue_work(fd);
            pthread_mutex_unlock(&queue_lock);
        }
    }

    //  Exit the pthread and return function
    pthread_exit(NULL);
    return NULL;
}

/*  Main function   */
int main( int argc, char *argv[] ){
    //  Create worker and listener thread
    pthread_t thread_listener;
    pthread_t thread_1;
    pthread_t thread_2;
    pthread_t thread_3;
    pthread_t thread_4;

    //  Initialize locks
    if(pthread_mutex_init(&num_delete_requests_lock, NULL) != 0){
        printf("Mutex failed to be created.\n");
        return 0;
    }
    if(pthread_mutex_init(&num_failed_requests_lock, NULL) != 0){
        printf("Mutex failed to be created.\n");
        return 0;
    }
    if(pthread_mutex_init(&num_read_requests_lock, NULL) != 0){
        printf("Mutex failed to be created.\n");
        return 0;
    }
    if(pthread_mutex_init(&num_write_requests_lock, NULL) != 0){
        printf("Mutex failed to be created.\n");
        return 0;
    }
    if(pthread_mutex_init(&queued_requests_lock, NULL) != 0){
        printf("Mutex failed to be created.\n");
        return 0;
    }
    if(pthread_mutex_init(&queue_lock, NULL) != 0){
        printf("Mutex failed to be created.\n");
        return 0;
    }
    if(pthread_mutex_init(&table_lock, NULL) != 0){
        printf("Mutex failed to be created.\n");
        return 0;
    }

    //  Add port number
    int port;
    if( argc < 2 ){
        port = 5000;
    }
    else if(argc == 2){
        port = atoi( argv[1] );
    }

    //  Remove all /tmp/data files
    system("rm -f /tmp/data.*");

    //  Initialize search table as empty strings
    table = malloc(sizeof(table_entry*)*SIZE);
    for (int i = 0; i < SIZE; i++){
        table_entry *entry = malloc(sizeof(table_entry));
        entry->name = malloc(sizeof(char)*31);
        entry->valid_bit = 'i';
        table[i] = entry;
    }

    //  Initailize work queue to nothing
    q = malloc(sizeof(queue_t));
    q->count = 0;
    q->first = NULL;
    q->last = NULL;

    //  Initialize line
    char line[128];

    //  Print intro statement
    if(DEBUG == 2 || DEBUG == 1 || DEBUG == 3){
        printf("---------------------------------------------\n");
        printf("\tSimple Multithread Socket Server\n");
        printf("---------------------------------------------\n\n");
    }

    //  Create listener thread and execute listener() func
    if(DEBUG == 2 || DEBUG == 1 || DEBUG == 3){
        printf("Creating threads.\n");
        printf("\tCreating listener thread...\n");
    }
    if(pthread_create(&thread_listener, NULL, listener, &port)) {
        printf("\tError creating listener thread\n");
        return 0;
    }
    if(DEBUG == 2 || DEBUG == 1 || DEBUG == 3){
        printf("\tCreating worker thread 1...\n");
    }
    //  Spawn 4 worker threads and execute worker_thread_func
    if(pthread_create(&thread_1, NULL, worker_thread_func, NULL)) {
        printf("\tError creating worker thread 1\n");
        return 0;
    }
    if(DEBUG == 2 || DEBUG == 1 || DEBUG == 3){
        printf("\tCreating worker thread 2...\n");
    }
    if(pthread_create(&thread_2, NULL, worker_thread_func, NULL)) {
        printf("\tError creating worker thread 2\n");
        return 0;
    }
    if(DEBUG == 2 || DEBUG == 1 || DEBUG == 3){
        printf("\tCreating worker thread 3...\n");
    }
    if(pthread_create(&thread_3, NULL, worker_thread_func, NULL)) {
        printf("\tError creating worker thread 3\n");
        return 0;
    }
    if(DEBUG == 2 || DEBUG == 1 || DEBUG == 3){
        printf("\tCreating worker thread 4...\n\n");
    }
    if(pthread_create(&thread_4, NULL, worker_thread_func, NULL)) {
        printf("\tError creating worker thread 4\n");
        return 0;
    }

    //  Infinite loop
    while (fgets(line, sizeof(line), stdin) != NULL) {
        //  Initialize word
        char word[8];

        //  Scan line into word
        sscanf(line, "%s", word);

        //  Compare keyword for stats
        if (strcmp(word, "stats") == 0){
            //  print title
            printf("\n---------------------------------------------\n");
            printf("Stats:\n");

            //  Call stats function
            stats_func();
        }

        //  Compare keyword for quit and exit
        else if ( strcmp(word, "quit") == 0){
            //  Exit and join pthreads
            if(DEBUG == 2 || DEBUG == 1 || DEBUG == 3){
                printf("Quiting Server.\n\n");
            }
            exit(0);
        }

        //  Empty word
        memset(word, 0, sizeof(word));
    }

    //  Return
    return 0;
}
