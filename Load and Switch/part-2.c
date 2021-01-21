/*
 * file:        part-2.c
 * description: Part 2, CS5600 load-and-switch assignment, Fall 2020
 */

/* NO OTHER INCLUDE FILES   */
#include "elf64.h"
#include "sysdefs.h"

/*  DEFINE  */
#define BUFFER 200
#define ROUND_UP(a,b) (((a+b-1)/b)*b)

/*  Global Variables    */
char *argv[BUFFER];
extern void *vector[];

extern void print(char *buf);
extern void readline(char *buf, int len);
extern char *getarg(int i);

/*  exit systemcall  */
void exit(int err){
    //  Call exit from syscall table -- 60
    syscall(err);
}

/*  read systemcall */
int read(int fd, void *ptr, int len){
    //  Call read from syscall table -- 0
    int answer = syscall(0, fd, ptr, len);

    //  Returns 0 or -1 if error
    return answer;
}

/*  write systemcall    */
int write(int fd, void *ptr, int len){
    //  Call write from syscall table -- 1
    int error_hand = syscall(1, fd, ptr, len);

    //  Returns 0 or -1 if error
    return error_hand;
}

/*  print a string to stdout (file descriptor 1)    */
void print(char *buf){
    //  Local variables
    int error_handle;
    int len = BUFFER;

    //  Enter write() with first val
    error_handle = write(1, buf, 1);

    //  Send byte for byte to write() function
    while ( len > 0 && *buf != '\n' && *buf++){
        //  Enter write() with new tmp val
        error_handle = write(1, buf, 1);

        //  Check for errors
        if (error_handle < 0){
            exit(60);
        }

        //  Break atend line
        if (*buf == '\0'){
            break;
        }

        //  Decrement length to ensure it's within buffer
        len--;
    }

    //  Return void
    return;
}

/*  read one line from stdin (file descriptor 0) into a buffer: */
void readline(char *buf, int len){
    //  Local variables
    int error_handle;
    char *tmp = buf;

    //  Ensures that we stay within the buffer; ensure there is one more byte at the end for \0
    while (--len > 1){
        //  Syscall to read user input
        error_handle = read(0, buf, 1);

        //  Check for errors in read
        if (error_handle < 0){
            exit(60);
        }

        //  Check for newline character and break
        if (*buf == '\n'){
            break;
        }

        //  Increment the arrays
        *tmp++ = *buf++;
    }

    //  Add \0 at the end of the buf[]
    *buf = '\0';

    //  Return buff size for buf[]
    return;
}

/*  write systemcall    */
int open(char *path, int flags){
    //  Call write from syscall table -- 2
    int error_hand = syscall(2, path, flags); 

    //  Returns 0 or -1 if error
    return error_hand;
}

/*  close systemcall    */
int close(int fd){
    //  Call write from syscall table -- 3
    int error_hand = syscall(3, fd);

    //  Returns 0 or -1 if error
    return error_hand;
}

/*  lseek systemcall    */
int lseek(int fd, int offset, int flag){
    //  Call write from syscall table -- 8
    int error_hand = syscall(8, fd, offset, flag);

    //  Returns offset or -1 if error
    return error_hand;
}

/*  mmap systemcall */
void *mmap(void *addr, int len, int prot, int flags, int fd, int offset){
    //  Call write from syscall table -- 9
    void *error_hand = (void *) syscall(9, addr, len, prot, flags, fd, offset);

    //  Returns pointer to mapped area or MAP_FAILED if error
    return error_hand;
}

/*  munmap systemcall   */
int munmap(void *addr, int len){
    //  Call write from syscall table -- 11
    int error_hand = syscall(11, addr, len);

    //  Returns 0 or -1 if error
    return error_hand;
}

//  do_getarg function
char *getarg(int i){
    return argv[i];
}

//  mapping function
int mapping(int fd){
    //  Error handling variable
    int error_handle;

    //  Read the ELF header
    struct elf64_ehdr hdr;
    read(fd, &hdr, sizeof(hdr));

    //  Sets number of program sections
    int n = hdr.e_phnum;
    struct elf64_phdr phdrs[n];

    //  Holding array
    void *holding_addr[n + 1];
    int holding_len[n + 1];

    //  Aquires offset
    lseek(fd, hdr.e_phoff, SEEK_SET);
    read(fd, phdrs, sizeof(phdrs));

    //  for each section, if b_type == PT_LOAD:
    for (int j = 0; j < n; j++){
        //  create mmap region
        if (phdrs[j].p_type == PT_LOAD) {
            //  read from file into region
            // len will be the amount of memory required rounded up to nearest 4096
            int len = ROUND_UP(phdrs[j].p_memsz, 4096);

            // allocate memory at recommended address an set it to be the
            // virutal address of the program section
            phdrs[j].p_vaddr = mmap((0x80000000 + phdrs[j].p_vaddr), len,
              PROT_READ | PROT_WRITE | PROT_EXEC,
              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            // go to the entry point of the program section, currently
            // getting an error from this function which i believe is causing
            // the seg fault when the function call is made on 142
            lseek(fd, (int) phdrs[j].p_offset, SEEK_SET);

            // read the executable file into the program memory
            read(fd, phdrs[j].p_vaddr, phdrs[j].p_filesz);

            //  store memory address and length being stored
            holding_addr[j] = phdrs[j].p_vaddr;
            holding_len[j] = len;
	    }
    }
    //  get function entry address
    void (*f)();
    f = hdr.e_entry + 0x80000000;
    f();

    //  munmap each mmap'ed region so we don't crash the 2nd time
    //  for each section, if b_type == PT_LOAD:
    for (int j = 0; j < n; j++){
        //  release mmap regions
        if (phdrs[j].p_type == PT_LOAD) {
            error_handle = munmap((void *) holding_addr[j], holding_len[j]);
            if (error_handle < 0){
                exit(60);
            }
        }
    }

    //  Return function
    return 0;
}

/* simple function to split a line:
 *   char buffer[200];
 *   <read line into 'buffer'>
 *   char *argv[10];
 *   int argc = split(argv, 10, buffer);
 *   ... pointers to words are in argv[0], ... argv[argc-1]
 */
int split(char **argv, int max_argc, char *line){
    int i = 0;
    char *p = line;

    while (i < max_argc) {
        while (*p != 0 && (*p == ' ' || *p == '\t' || *p == '\n'))
            *p++ = 0;
        if (*p == 0)
            return i;
        argv[i++] = p;
        while (*p != 0 && *p != ' ' && *p != '\t' && *p != '\n')
            p++;
    }
    return i;
}

/* testing function for quit */
void testing_quit(char *word){
    //  Compare strings for 'quit'
    int testing = 0;

    if (word[0] == 'q'){
        testing++;
    }
    if (word[1] == 'u'){
        testing++;
    }
    if (word[2] == 'i'){
        testing++;
    }
    if (word[3] == 't'){
        testing++;
    }

    if( testing == 4){
        exit(60);
    }

    //  returns void
    return;
}

/* main */
void main(void){
    //  Vector array
    vector[0] = readline;
    vector[1] = print;
    vector[2] = getarg;

    //  Local variables
    char input[BUFFER];
    int argc;
    int fd;

    //  Prompt
    print("Hello, type lines or 'quit':\n\0");

    //  Loop through forever
    while ( 1 ){
        //  Print > character
        print("> \0");

        //  Read line to input array
        readline(input, BUFFER);

        //  Split input array into words
        argc = split(argv, 10, input);

        //  Check if newline character only given
        if(argc != 0){
            //  Test if first word is quit
            testing_quit(argv[0]);

            //  First word; path name
            char *path = argv[0];

            //  Load file into memory
            if ((fd = open(path, O_RDONLY)) >= 0){
                //  Map file to memory
                mapping(fd);

                //  Close file
                close(fd);
            }
        }
    }

    //  Cleanly exit function
    exit(60);
}
