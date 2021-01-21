/*
 * file:        part-1.c
 * description: Part 1, CS5600 load-and-switch assignment, Fall 2020
 */

/* THE ONLY INCLUDE FILE */
#include "sysdefs.h"

/*  DEFINE  */
#define BUFFER 200


/* exit systemcall*/
void exit(int err){
    //  Call exit from syscall table
    syscall(err);
}

/* read systemcall */
int read(int fd, void *ptr, int len){
    //  Call read from syscall table
    int answer = syscall(0, fd, ptr, len);

    //  Returns value in bytes
    return answer;
}

/* write systemcall */
int write(int fd, void *ptr, int len){
    //  Call write from syscall table
    int error_hand = syscall(1, fd, ptr, len);

    //  Returns value in bytes
    return error_hand;
}

/* print a string to stdout (file descriptor 1) */
int print(char input[], int len){
    //  Local variables
    int error_handle;

    //  Compare strings for 'quit'
    int testing = 0;
    if (input[0] == 'q'){
        testing++;
    }
    if (input[1] == 'u'){
        testing++;
    }
    if (input[2] == 'i'){
        testing++;
    }
    if (input[3] == 't'){
        testing++;
    }

    if( testing == 4){
        exit(60);
    }

    //  Enter write() with first val
    error_handle = write(1, input, 1);

    //  Send byte for byte to write() function
    while ( len > 0 && *input != '\n' && *input++){
        //  Enter write() with new tmp val
        error_handle = write(1, input, 1);

        //  Check for errors
        if (error_handle < 0){
            exit(60);
        }

        //  Decrement length to ensure it's within buffer
        len--;
    }

    //  Return the comparison of byte length of arrays to ensure no error
    return len;
}

/* read one line from stdin (file descriptor 0) into a buffer: */
int readline(char input[], int len){
    //  Local variables
    int error_handle;
    int buff = 0;
    char *tmp = input;

    //  Ensures that we stay within the buffer; ensure there is one more byte at the end for \0
    while (--len > 1){
        //  Syscall to read user input
        error_handle = read(0, input, 1);

        //  Check for errors in read
        if (error_handle < 0){
            exit(60);
        }

        //  Check for newline character and break
        if (*input == '\n'){
            break;
        }

        //  Increment buffer
        buff++;

        //  Increment the arrays 
        *tmp++ = *input++;
    }

    //  Add \0 at the end of the input[]
    *input = '\0';
    buff++;

    //  Return buff size for input[]
    return buff;
}

/* main */
void main(int argc, char *argv[]){
    //  Local variables
    char input[BUFFER];
    int buff;

    //  Prompt
    print("Hello, type lines or 'quit':\n", 29);

    //  Loop through forever
    while ( 1 ){
        //  print > character
        print("> ", 2);

        //  read line to input array
        buff = readline(input, BUFFER);

        //  print input array
        print(input, buff); 

        //  Print newline character
        print("\n", 1);

        //  Empty array
        while ( --buff > 0){
            input[buff - 1] = 0;
        }
    }

    //  cleanly exit function
    exit(60);
} 