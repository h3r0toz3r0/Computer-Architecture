/*
Project 0: Getting Started 
    Part 3 Ciphers
Author: Anna DeVries
Date: 21 September 2020

Useage: ./caesar <message> <decode/encode> <shift_value>
*/

//  Includes
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Defines
#define BUFFER 50

//  Encode() encodes the string plaintext using Caesar cipher by shifting characters by key positions.
//  Returns encoded text in all upper case.
void encode(const char *plaintext, int key){
    //  Local variables
    char character; 

    //  Capitalize character
    character = toupper(*plaintext);

    //  Rotate letter
    if (character >= 'A' && character <= 'Z'){
        character = character + key;

        //  Check letters that must circle around ASCII numbers
        if (character > 'Z'){
            character = character + 'A' - 'Z' - 1;
        }
    }

    //  Print results
    printf("%c", character);
}

//  Decode() decodes the string ciphertext using Caesar cipher by shifting the charachters back by key positions.
//  Returns decoded text in all upper case.
void decode(const char *ciphertext, int key){
    //  Local variables
    char character; 

    //  Capitalize character
    character = toupper(*ciphertext);

    //  Rotate letter
    if (character >= 'A' && character <= 'Z'){ 
        character = character - key;
    
        //  Check letters that must circle around ASCII numbers
        if (character < 'A'){
            character = character - 'A' + 'Z' + 1;
        }
    }

    //  Print results
    printf("%c", character);

    return;
}

//  Main function
int main(int argc, char* argv[]){
    //  Check for command line arguments
    if (argc != 4){
        printf("\nUseage: gcc caesar.c -o caesar && ./caesar <text> <decode/encode> <shift_value>\n");
        printf("Exiting.\n\n");
        return 0;
    }

    //  local variables
    char *text = argv[1];
    char *encrypt = argv[2]; 
    int key = atoi(argv[3]);

    //  Print info
    printf("\nCaesar Cipher Tool\n");
    printf("Shift key = %d\n\n", key);

    //  Determine if encrypting or decrypting
    if (strcmp(encrypt, "encode") == 0){
        //  Encoding 
        printf("Encrypting %s\n", text);
        printf("Ciphertext: ");

        //  Iterate through each character
        for( int i = 0; i < BUFFER; i++){
            if( text[i] == '\0'){
                printf("\n\n");
                return 0;
            }
            encode(&text[i], key);
        }
    }

    else if (strcmp(encrypt, "decode") == 0){
        //  Decoding 
        printf("Decrypting %s\n", text);
        printf("Plaintext: ");

        //  Iterate through each character
        for( int i = 0; i < BUFFER; i++){
            if( text[i] == '\0'){
                printf("\n\n");
                return 0;
            }
            decode(&text[i], key);
        }
    }

    //  Exit if neither decode or encode are selected
    else{
        printf("Useage: gcc caesar.c -o caesar && ./caesar <text> <decode/encode> <shift_value>\n");
        printf("Exiting.\n\n");
    }

    //  Return and exit
    return 0;
    
}

