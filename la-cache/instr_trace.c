/*	Cache Archiitecture Reversing
	EECE 7352 Computer Architecture

	by Anna DeVries
	5 November 2019

*/

/*	Instr streams generated are for DineroIV cache simulator.
	LABEL 	= 0 read data
		= 1 write data
		= 2 instruction fetch
		= 3 escape record (treated as unknown access type)
		= 4 escape record (causes cache flush)
0 <= ADDRESS <= ffffffff where the hexadecimal addresses are NOT preceded by "0x."
	Here are some examples:
		2 0 This is an instruction fetch at hex address 0.
		0 1000 This is a data read at hex address 1000.
		1 70f60888 This is a data write at hex address 70f60888

	dineroIV –l1-isize 16K –l1-ibsize 32 –l1-dsize 16K –l1-dbsize 32 -informat d < trace.txt
*/

#include <stdio.h>

// Cache: 64KB, 64B block size, direct-mapped
// Generates address stream that will touch every cache line just once
int partOne(){
    long addr = 0x000000;
    int i;
    for(i = 0; i < 1024; i++){
        printf("2 F%x0 \n", addr);
        addr = addr + 64;
    }
    return 0;
} 

// Cache: 64KB, 64B block size, 2-way set associative, LRU replacement
// Generates address stream that will touch every cache line 7 times
// producing 3 misses and 4 hits but only 3 unique addresses per line
int partTwo(){
    long addr = 0x000000;
    int i;
    for(i = 0; i < 512; i++){
        printf("2 F%x0 \n", addr);
        printf("2 F%x0 \n", addr);
        printf("2 E%x0 \n", addr);
        printf("2 E%x0 \n", addr);
        printf("2 D%x0 \n", addr);
        printf("2 D%x0 \n", addr);
        printf("2 D%x0 \n", addr);
        addr = addr + 64;
    }
    return 0;
}

// Cache: 64KB, 64B block size, 2-way set associative, LRU replacement
// Generates address stream that will touch every cache line 10 times
// producing 5 misses and 5 hits but only 3 unique addresses per line
// pattern must be Miss-Hit-Miss-Hit...
int partThree(){
    long addr = 0x000000;
    int i;
    for(i = 0; i < 512; i++){
        printf("2 F%x0 \n", addr);
        printf("2 F%x0 \n", addr);
        printf("2 D%x0 \n", addr);
        printf("2 F%x0 \n", addr);
        printf("2 E%x0 \n", addr);
        printf("2 F%x0 \n", addr);
        printf("2 D%x0 \n", addr);
        printf("2 D%x0 \n", addr);
        printf("2 E%x0 \n", addr);
        printf("2 E%x0 \n", addr);
        addr = addr + 64;
    }
    return 0;
}

// Cache: 64KB, 32B block size, 4-way set associative, LRU replacement
// Generates address stream that will touch every cache line 8 times
// producing 3 misses and 5 hits
// must include loads and stores
int partFour(){
    long addr = 0x000000;
    int i;
    for(i = 0; i < 256; i++){
        printf("1 F%x0 \n", addr); // 0 = read data, 1 = write data
        printf("0 F%x0 \n", addr);
        printf("0 F%x0 \n", addr);
        printf("0 F%x0 \n", addr); 
        printf("1 D%x0 \n", addr); 
        printf("0 D%x0 \n", addr);
        printf("0 D%x0 \n", addr);
        printf("0 E%x0 \n", addr); 
        addr = addr + 32;
    }
    return 0;
}

// Cache: <32KB, 16B block size, 4-way set associative, LRU replacement
// Generate 1+ address stream to detect set associativity and instr cache size
int partFive_assoco(){
    long addr = 0x000000;
    int i;
    for(i = 0; i < 2048; i++){
        printf("0 F%x0 \n", addr);
        printf("2 F%x0 \n", addr);
        printf("2 D%x0 \n", addr);
        printf("2 F%x0 \n", addr);
        printf("2 E%x0 \n", addr);
        printf("2 D%x0 \n", addr);
        addr = addr + 32;
    }
    return 0;
}

int partFive_size(){
    long addr = 0x000000;
    int i;
    for(i = 0; i < 256; i++){
        printf("2 F%x0 \n", addr);
        addr = addr + 32;
    }
    for(i = 256; i > 0; i--){
        printf("2 F%x0 \n", addr);
        addr = addr - 32;
    }
    for(i = 0; i < 256; i++){
        printf("2 F%x0 \n", addr);
        addr = addr + 32;
    }
    for(i = 256; i > 0; i--){
        printf("2 F%x0 \n", addr);
        addr = addr - 32;
    }
    for(i = 0; i < 256; i++){
        printf("2 F%x0 \n", addr);
        addr = addr + 32;
    }
    for(i = 256; i > 0; i--){
        printf("2 F%x0 \n", addr);
        addr = addr - 32;
    }
    for(i = 0; i < 256; i++){
        printf("2 F%x0 \n", addr);
        addr = addr + 32;
    }
    for(i = 256; i > 0; i--){
        printf("2 F%x0 \n", addr);
        addr = addr - 32;
    }
    return 0;
}

// Cache: <32KB, 16B block size, 4-way set associative, LRU replacement
// Generate a address stream to determine resplacement algorithm
int partSix(){
    long addr = 0x000000;
    int i;

    printf("2 A%x0 \n", addr);      // LRU: M, M, H, M, H, M
    printf("2 B%x0 \n", addr);      // FIFO: M, M, H, M, H, H
    printf("2 A%x0 \n", addr);
    printf("2 C%x0 \n", addr);
    printf("2 C%x0 \n", addr);
    printf("2 B%x0 \n", addr);

    return 0;
}

int main(){
    partOne();

    return 0;
}
