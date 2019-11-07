/*      Cache Simulation
        EECE 7352 - Computer Architecture

        by Anna DeVries
        5 November 2019

        Usage:
        ./cache <trace file>

        Trace File Format:
        LABEL   = 0       read data               
                = 1       write data               
                = 2       instruction fetch               
                = 3       escape record (treated as unknown access type)               
                = 4       escape record (causes cache flush) 
        0 <= ADDRESS <= ffffffff where the hexadecimal addresses are NOT preceded by "0x." 
        
        Example Trace File Formats:
        2 0    This is an instruction fetch at hex address 0. 
        0 1000   This is a data read at hex address 1000. 
        1 70f60888  This is a data write at hex address 70f60888. 

        Description:
        8KB instruction cache
        32B block size
        32-bit address
        Column associative structure
        Reports miss rate               */

/*      Libraries to include            */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

/*      Global defines                  
   -----------------------------------------------------
  | Tag: 19 bits | Index: 8 bits | Byte Offset: 5 bits |
   -----------------------------------------------------
  31           13 12            5 4                    0 
*/
#define SIZE 8192                       /* cache size in bytes */
#define BSIZE 32                        /* block size in bytes */
#define ASIZE 32                        /* address size in bits */
#define OFFSET 5                        /* log10(BSIZE)/log10(2) */
#define INDEX 8                         /* log10((SIZE/BSIZE))/log10(2) - 0 */
#define TAG 19                          /* BSIZE - calc2 - calc1 */


/*      Typedefs                        */
typedef struct Block_* Block;
typedef struct Cache_* Cache;
typedef struct Instruction_* Instruction;

/*      Struct objects                  */
// Block object 
struct Block_{
    int tag;
};

// Cache object 
struct Cache_{
    int hits, misses, size, bsize, lines;
    Block* blocks;
};

// Instruction object 
struct Instruction_{
    int command;
    int addr;
    int tag, index, offset;
};

/*      Utility functions               */
Instruction create_instr(){
    // Variables 
    Instruction instruction;

    // Allocate memory for instructions 
    instruction = (Instruction) malloc(sizeof(struct Instruction_));
    if(instruction == NULL){
        printf("No memory allocated for cache.\n");

        return 0;
    }

    // Initalize instruction variables
    (*instruction).command = 0;
    (*instruction).addr = 0;
    (*instruction).tag = 0;
    (*instruction).offset = 0;
    (*instruction).index = 0;

    return instruction;
}

char *hex_to_binary(int address){
    // Variables 
    Instruction instruction;
    char hex_buffer[9];
    char *bin_buffer = (char *)malloc(sizeof(char)*ASIZE);

    // Convert int address into a string
    sprintf(hex_buffer, "%x", address);

    strncpy(bin_buffer,"", sizeof(bin_buffer) - 1);

    // Convert string hex address to string binary
    for(int i = 0; i < sizeof(hex_buffer); i++){
        if(hex_buffer[i] == '0'){
            strcat(bin_buffer, "0000");
        }
        else if(hex_buffer[i] == '1'){
            strcat(bin_buffer, "0001"); 
        }
        else if(hex_buffer[i] == '2'){
            strcat(bin_buffer, "0010"); 
        }
        else if(hex_buffer[i] == '3'){
            strcat(bin_buffer, "0011"); 
        }
        else if(hex_buffer[i] == '4'){
            strcat(bin_buffer, "0100"); 
        }
        else if(hex_buffer[i] == '5'){
            strcat(bin_buffer, "0101"); 
        }
        else if(hex_buffer[i] == '6'){
            strcat(bin_buffer, "0110"); 
        }
        else if(hex_buffer[i] == '7'){
            strcat(bin_buffer, "0111"); 
        }
        else if(hex_buffer[i] == '8'){
            strcat(bin_buffer, "1000"); 
        }
        else if(hex_buffer[i] == '9'){
            strcat(bin_buffer, "1001"); 
        }
        else if(hex_buffer[i] == 'a'){
            strcat(bin_buffer, "1010"); 
        }
        else if(hex_buffer[i] == 'b'){
            strcat(bin_buffer, "1011"); 
        }
        else if(hex_buffer[i] == 'c'){
            strcat(bin_buffer, "1100"); 
        }
        else if(hex_buffer[i] == 'd'){
            strcat(bin_buffer, "1101"); 
        }
        else if(hex_buffer[i] == 'e'){
            strcat(bin_buffer, "1110"); 
        }
        else if(hex_buffer[i] == 'f'){
            strcat(bin_buffer, "1111"); 
        } 
    }

    return bin_buffer;
}

int binary_to_int(char *bin_buffer, int size){  
    // Variables
    int sum = 0;
    int base = 1; 

    // Convert binary to integer
    for(int i = size - 1; i >= 0; i--){ 
        if (bin_buffer[i] == '1') 
            sum += base; 
        base = base * 2; 
    } 

    return sum;
}

/*      Cache functions                 */
Cache create(){
    /* Variables */
    Cache cache;

    /* Allocate memory for cache */
    cache = (Cache) malloc(sizeof(struct Cache_));
    if(cache == NULL){
        printf("No memory allocated for cache.\n");

        return 0;
    }

    /* Initalize cache variables*/
    (*cache).hits = 0;
    (*cache).misses = 0;
    (*cache).size = SIZE;
    (*cache).bsize = BSIZE;
    (*cache).lines = SIZE / BSIZE;
    (*cache).blocks = (Block*) malloc(sizeof(Block) * (*cache).lines);
    assert((*cache).blocks != NULL);

    /* Initalize blocks */
    for(int i = 0; i < (*cache).lines; i++ ){
        (*cache).blocks[i] = (Block) malloc(sizeof(struct Block_ ));
        assert((*cache).blocks[i] != NULL);
        (*(*cache).blocks[i]).tag = 0;
    }

    return cache;
}

Cache destroy(Cache cache){
    free((*cache).blocks);
    free(cache);
    cache = NULL;

    return cache;
}

int play_with_cache(Cache cache, int offset, int index, int tag){
    // Variables
    Block block;
    Block new_block;
    int c, k;
    char new_index[INDEX];

    // Get block
    block = (*cache).blocks[index];

    // Compare block tag value with current tag at index
    // Hit
    if((*block).tag == tag){
        (*cache).hits++;

        return 1;
    }
    // Miss
    else{
        // Block is empty, add value
        if((*block).tag == 0){
            (*cache).misses++;
            (*block).tag = tag;

            return 1;
        }

        // Block is full, try another hashing function
        // Hashing function = index bit-flipping
        // Convert index back to binary, and flip bits
        strncpy(new_index, "", sizeof(new_index) - 1);

        int i = 0;
        for(c = INDEX - 1; c >= 0; c--){
            k = index >> c;
 
            if (k & 1){                 /* Replace 1 with 0 for flipped bits*/
                strcat(new_index, "0");
            }
            else{                       /* Replace 0 with 1 for flipped bits*/
                strcat(new_index, "1");
            }
            i++;
        }

        // Convert new_index binary string into integer
        int n_index = binary_to_int(new_index, INDEX);

 
        // Get new block
        new_block = (*cache).blocks[n_index];

        // Compare block tag value with current tag at index
        // Hit
        if((*new_block).tag == tag){
            (*cache).hits++;

            // Switch block tags
            (*new_block).tag = (*block).tag;
            (*block).tag = tag;

            return 1;
        }
        else{
            (*cache).misses++;
            (*new_block).tag = tag;
        }

    }

}

/*      Main function                   */
int main(int argc, char **argv){
    // Intro
    printf("\nWelcome to La Cache - a Column Associative Cache Simulation\n");
    printf("by Anna DeVries\n");
    printf("5 November 2019\n\n");


    // Variables 
    FILE *fp;
    Cache cache;
    Instruction instruction;
    char line[100];
    char *bin_buffer;

    // Check arguments 
    if(argc < 2){
        printf("Usage: ./cache <trace file>\n");
        printf("For help: ./cache --help\n");

        return 0;
    }

    // Check if help is requested
    if(strcmp(argv[1], "--help") == 0){
        printf("\nUsage: ./cache <trace file>\n\n");
        printf("Trace File Format:\nLABEL   = 0       read data\n= 1       write data\n= 2       instruction fetch\n= 3       escape record (treated as unknown access type)\n= 4       escape record (causes cache flush)\n0 <= ADDRESS <= ffffffff where the hexadecimal addresses are NOT preceded by 0x.\n\nExample Trace File Formats:\n2 0    This is an instruction fetch at hex address 0.\n0 1000   This is a data read at hex address 1000.\n1 70f60888  This is a data write at hex address 70f60888.\n");
        printf("\nCache Simulation Description:\n");
        printf("%dB cache size, %dB block size, %d-bit addresses, column associative\n\n", SIZE, BSIZE, ASIZE);
        printf("\nCache simulation created by Anna DeVries\n");
        return 0;
    }

    // Open trace file 
    fp = fopen(argv[1], "r");

    // Check trace file 
    if(fp == NULL){
        printf("Error opening file.\n");

        return -1;
    }

    // Initialize cache 
    cache = create();

    // Initialize instruction 
    instruction = create_instr();

    // Begin instruction flow 
    // Grab instructions from each line 
    while(fgets(line, sizeof(line), fp) != NULL){
        // Parse values into instruction object 
        sscanf(line, "%d %x", &((*instruction).command), &((*instruction).addr));

        // Convert address to binary
        bin_buffer = hex_to_binary((*instruction).addr);

        // Format memory address
        // Format offset
        char *partial_offset = (char *)malloc(sizeof(char)*OFFSET);
        strncpy(partial_offset,"", sizeof(partial_offset) - 1);
        for(int i = OFFSET; i > 0; i--){
            partial_offset[OFFSET - i] = bin_buffer[TAG + INDEX + OFFSET - i];
        }
        // Convert binary to decimal int 
        (*instruction).offset = binary_to_int(partial_offset, OFFSET);

        // Format index
        char *partial_index = (char *)malloc(sizeof(char)*INDEX);
        strncpy(partial_index,"", sizeof(partial_index) - 1);
        for(int i = INDEX; i > 0; i--){
            partial_index[INDEX - i] = bin_buffer[TAG + INDEX - i];
        }
        // Convert binary to decimal int 
        (*instruction).index = binary_to_int(partial_index, INDEX);

        // Format tag
        char *partial_tag = (char *)malloc(sizeof(char)*TAG);
        strncpy(partial_tag,"", sizeof(partial_tag) - 1);
        for(int i = TAG; i > 0; i--){
            partial_tag[TAG - i] = bin_buffer[TAG - i];
        }
        // Convert binary to decimal int 
        (*instruction).tag = binary_to_int(partial_tag, TAG);

        // Check if command reads (0), writes (1) or fetches (2) data. Only concerned about instruction fetches.
        if((*instruction).command == 2){ 
            play_with_cache(cache, (*instruction).offset, (*instruction).index, (*instruction).tag);

        }
    }

    // Print results
    int total = (*cache).hits + (*cache).misses;
    float hit_rate = ((float)(*cache).hits / (float)total) * 100;
    float miss_rate = ((float)(*cache).misses / (float)total) * 100;

    printf("\nResults: \n");
    printf("--------------------------------------------------------\n");
    printf("Instruction Cache\n");
    printf("Description: %dB cache size, %dB block size, %d-bit addresses, column associative\n\n", SIZE, BSIZE, ASIZE);
    printf("Cache Hits:     %d (%.2f %%)\n", (*cache).hits, hit_rate);
    printf("Cache Misses:    %d (%.2f %%)\n", (*cache).misses, miss_rate);
    printf("Total Fetches:    %d\n\n", total);


    /* Gracefully terminate */
    fclose(fp);
    destroy(cache);

    return 1;

}

