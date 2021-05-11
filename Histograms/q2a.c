/*
 * Title: General 2000-ball histogram
 * Author: Anna DeVries
 * Date: 17 March 2021
 *
 * Description:    generates a 2000-ball histogram such that 
 *                  2n = 2000 balls are randomly placed into 
 *                  n = 1000 boxes
 * 
 * Useage:      gcc -c rng.c
 *              gcc -lm <file>.c rng.o -o <file>
*/

//  Libraryes
#include <stdio.h>
#include <math.h>                                             
#include "rng.h"                          

//  Macros
#define DEBUG 0

//  Generates Equilikely random variate, use a < b
long Equilikely(long a, long b){
    return (a + (long) ((b - a + 1) * Random()));
}

//  Main Function
int main(void){
    // Local variables
    int n = 1000;
    int b = n *2;
    int S[n];
    int i, j;

    // Intro
    printf("\nHistogram Problem\n");
    printf("Author: Anna DeVries\n");
    printf("Date: 17 March 2021\n");
    printf("Description: %d balls are randomly thrown into %d containers\n", b, n);
    printf("Output: histogram mean and standard deviation\n");
    printf("---------------------------------------------------------------------------------------\n\n");

    // Random seed
    PutSeed(12345);

    // Assign "Empty" boxes
    for ( i = 0; i < n; i++ ){
        S[i] = 0;
    }

    // Monte Carlo Simulation to Assign Balls Randomly
    int max = 0;
    for ( j = 0; j < b; j++ ){
        i = Equilikely(1, n);
        S[i]++;
        if( S[i] > max )
            max = S[i];
    }

    // Debug only
    if (DEBUG == 1){
        printf("\tContainers {box id:ball count} \n");
        for ( i = 1; i <= n; i++ ){
            printf("{%d:%d}\t", i, S[i-1]);
            if (i % 30 == 0){
                printf("\n");
            }
        }
        printf("\n");
    }

    // Create empty frequency and RF histograms
    float frequency[max+1];
    float rf[max+1];
    for ( i = 0; i < (max+1); i++){
        frequency[i] = 0;
        rf[i] = 0;
    }

    // Add values to frequency
    for ( i = 0; i < n; i++ ){
        frequency[S[i]]++;
    }

    // Debug only
    if (DEBUG == 1){
        printf("\n\tFrequency Histogram {ball count:frequency}\n");
        for ( i = 1; i <= max+1; i++ )
            printf("{%d:%3.0f}\t", i-1, (frequency[i-1]));
    }

    // Debug only
    if (DEBUG == 1)
        printf("\n\tRelative Frequency Histogram {ball count:RF}\n");

    // Calculate relative frequency histogram
    for ( i = 1; i <= max+1; i++ ){
        rf[i-1] = frequency[i-1] / n;

        // Debug only
        if (DEBUG == 1){
            printf("{%d:%3.3f}\t", i-1, (rf[i-1]));
        }
    }

    // Debug only
    if (DEBUG == 1)
        printf("\n\n");

    // Debug only
    if (DEBUG == 1)
        printf("Running Histogram Mean and Standard Deviation:\n");

    // Initialize mean and standard deviation variables 
    float x = 0;
    float s = 0;

    // Loop through histogram
    for ( i = 0; i <= max; i++ ){
        // Calculate mean
        x += i * rf[i];

        // Debug only 
        if ( DEBUG == 1)
            printf("\tx: %d\trf: %3.2f\trunning mean: %3.2f\n", i, rf[i], x);
    }

    // Loop through histogram
    for ( i = 0; i <= max; i++ ){
        // Calculate standard deviation
        s += (((float)i - x)*((float)i - x))*rf[i];

        // Debug only 
        if ( DEBUG == 1)
            printf("\tx: %d\trf: %3.2f\trunning standard variance: %3.2f\n", i, rf[i], s);
    }
    s = sqrt(s);

    // Debug only
    if ( DEBUG == 1)
        printf("\n");

    // Loop through histogram
    printf("Histogram {x, f^(x)}:\t\t");
    for ( i = 1; i <= max+1; i++ ){
        printf("{%d, %3.3f}\t", i-1, rf[i-1]);
        if( i % 5 == 0)
            printf("\n\t\t\t\t");
    }
    printf("\n");
    printf("Histogram Mean : \t\t%3.2f\n", x);
    printf("Histogram Standard Deviation : \t%3.3f\n\n", s);

    return 0;
}
