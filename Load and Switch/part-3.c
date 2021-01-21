/*
 * file:        part-3.c
 * description: part 3, CS5600 load-and-switch assignment, Fall 2020
 */

/* NO OTHER INCLUDE FILES */
#include "elf64.h"
#include "sysdefs.h"

/*  DEFINE  */
#define BUFFER 200
#define ROUND_UP(a,b) (((a+b-1)/b)*b)

/**/
extern void *vector[];
char stack1[4096];
char stack2[4096];
char calling_stack[4096];
extern void switch_to(void **location_for_old_sp, void *new_value);
extern void *setup_stack0(void *_stack, void *func);

/* ---------- */

/* exit systemcall*/
void exit(int err){
    //  Call exit from syscall table -- 60
    syscall(err);
}

/* read systemcall */
int read(int fd, void *ptr, int len){
    //  Call read from syscall table -- 0
    int answer = syscall(0, fd, ptr, len);

    //  Returns 0 or -1 if error
    return answer;
}

/* write systemcall */
int write(int fd, void *ptr, int len){
    //  Call write from syscall table -- 1
    int error_hand = syscall(1, fd, ptr, len);

    //  Returns 0 or -1 if error
    return error_hand;
}

/* print a string to stdout (file descriptor 1) */
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

/* write systemcall */
int open(char *path, int flags){
    //  Call write from syscall table -- 2
    int error_hand = syscall(2, path, flags);

    //  Returns 0 or -1 if error
    return error_hand;
}

/* close systemcall */
int close(int fd){
    //  Call write from syscall table -- 3
    int error_hand = syscall(3, fd);

    //  Returns 0 or -1 if error
    return error_hand;
}

/* lseek systemcall */
int lseek(int fd, int offset, int flag){
    //  Call write from syscall table -- 8
    int error_hand = syscall(8, fd, offset, flag);

    //  Returns offset or -1 if error
    return error_hand;
}

/* mmap systemcall */
void *mmap(void *addr, int len, int prot, int flags, int fd, int offset){
    //  Call write from syscall table -- 9
    void *error_hand = (void *) syscall(9, addr, len, prot, flags, fd, offset);

    //  Returns pointer to mapped area or MAP_FAILED if error
    return error_hand;
}

//  mapping function
void *mapping(int fd, long address_offset){
    //  Read the ELF header
    struct elf64_ehdr hdr;
    read(fd, &hdr, sizeof(hdr));

    //  Sets number of program sections
    int n = hdr.e_phnum;
    struct elf64_phdr phdrs[n];

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
            phdrs[j].p_vaddr = mmap((address_offset + phdrs[j].p_vaddr), len,
              PROT_READ | PROT_WRITE | PROT_EXEC,
              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            // go to the entry point of the program section, currently
            // getting an error from this function which i believe is causing
            // the seg fault when the function call is made on 142
            lseek(fd, (int) phdrs[j].p_offset, SEEK_SET);

            // read the executable file into the program memory
            read(fd, phdrs[j].p_vaddr, phdrs[j].p_filesz);
	    }
    }
    //  get function entry address
    void (*f)();
    f = hdr.e_entry + address_offset;

    //  Return memory entry address
    return f;
}

/* ---------- */

/*  this switches from process 1 to 2 */
void do_yield12(void){
    //  Context switching
    void** sp1 = (void **) &stack1;
    void** sp2 = (void **) &stack2;
    switch_to((void **) sp1, (void*) *sp2);

    return;
}

/*  this switches from process 2 to 1 */
void do_yield21(void){
    //  Context switching
    void** sp1 = (void **) &stack1;
    void** sp2 = (void **) &stack2;
    switch_to((void **) sp2, (void*) *sp1);

    return;
}

/*  this switches back to the original process stack */
void do_uexit(void){
    //  Context switching
    void** sp1 = (void **) &stack1;
    void** calling = (void **) &calling_stack;
    switch_to((void **) sp1, *calling);

    return;
}

/* ---------- */

void main(void){
    vector[1] = print;

    vector[3] = do_yield12;
    vector[4] = do_yield21;
    vector[5] = do_uexit;

    //  Local variable
    int fd1, fd2;

    //  Set address offsets for process 1 and 2
    long address_offset1 = 0x10000000;
    long address_offset2 = 0x20000000;

    //  Load files into memory
    fd1 = open("process1", O_RDONLY);
    fd2 = open("process2", O_RDONLY);

    //  Get entry memory location for programs
    void (*map1)() = mapping(fd1, address_offset1);
    void (*map2)() = mapping(fd2, address_offset2);

    //  Setup stacks
    void** sp1 = (void **) &stack1;
    void** sp2 = (void **) &stack2;
    *sp1 = setup_stack0(stack1+4096, map1);
    *sp2 = setup_stack0(stack2+4096, map2);

    //  Switch to first process
    switch_to((void**) &calling_stack, *sp1);

    //  Close file
    close(fd1);
    close(fd2);

    //  Cleanly exit function
    print("done\n");
    exit(60);
}
