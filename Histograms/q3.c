/*
 * Title: Grade Predicition Histogram
 * Author: Anna DeVries
 * Date: 18 March 2021
 *
 * Description:     A test is compiled by selecting 12 different questions, at random
 *                  and without replacement, from a well-publicized list of 120 questions. 
 *                  After studying this list you are able to classify all 120 questions 
 *                  into two classes, I and II. Class I questions are those about which you 
 *                  feel confident; the remaining questions define class II. Assume that
 *                  your grade probability, conditioned on the class of the problem, is
 *                              A       B       C       D       F
 *                  class1      0.6     0.3     0.1     0.0     0.0
 *                  class2      0.0     0.1     0.4     0.4     0.1
 *                  Each test question is grade on an A = 4, B = 3, C = 2, D = 1, F = 0 scale 
 *                  and a score of 36 or better is required to pass the test.
 *                  This program assumes there are 90-class1 questions in the list and utilizes
 *                  Monte Carlo simulation and 100,000 replications to generate a discrete-data
 *                  histogram of sccores to predict the probability you will pass the test.
 * 
 * 
 * Useage:      gcc -c rng.c
 *              gcc -lm <file>.c rng.o -o <file>
*/

//  Libraryes
#include <stdio.h>
#include <stdlib.h>
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
    // Monte Carlo simulation variables
    int n = 100000;
    int i, j, k, z;

    // Intro
    printf("\nHistogram Problem\n");
    printf("Author: Anna DeVries\n");
    printf("Date: 18 March 2021\n");
    printf("Description: utilizes a Monte Carlo simulation to generate a discrete-data histogram of your grade probability\n");
    printf("Output: histogram and probability you will pass the test\n");
    printf("----------------------------------------------------------------------------------------------------------------\n\n");

    // Random seed
    PutSeed(12345);

    // Initialize grade probability distribution such as array[] = {A, B, C, D, F}
    float class1[] = {0.6, 0.3, 0.1, 0.0, 0.0};
    float class2[] = {0.0, 0.1, 0.4, 0.4, 0.1};
    float grades[] = {4.0, 3.0, 2.0, 1.0, 0.0};

    // Problem initialization
    int total_questions = 120;                                  // total questions on list that can be selected
    int class1_questions = 90;                                  // number of questions on list that are in class1
    int class2_questions = total_questions - class1_questions;  // number of questions on list that are in class2
    int test_questions = 12;                                    // number of questions on the test
    int passing_score = 36;                                     // necessary score to pass the test

    // Create an array to hold the pass or fail results
    int pass[n];
    for ( j = 0; j < n; j++)
        pass[j] = 0;

    // Create an array to hold the questions chosen
    int test[test_questions + 1];
    for ( j = 0; j <= test_questions; j++){
        test[j] = 0;
    }

    // Create an array to keep track of questions selected
    int selected_questions[total_questions + 1];
    for ( j = 0; j <= total_questions; j++){
        selected_questions[j] = 0;
    }

    // Allocate memory for monte carlo array
    int*** monte_carlo_tests = (int***)malloc(n * sizeof(int**));
    if (monte_carlo_tests == NULL) {
        exit(0);
    }
    for ( i = 0; i < n; i++){
        monte_carlo_tests[i] = (int**)malloc(n * sizeof(int*));
        if (monte_carlo_tests[i] == NULL) {
            exit(0);
        }
        for ( j = 0; j <= test_questions; j++){
            monte_carlo_tests[i][j] = (int*)malloc(2 * sizeof(int));
               if (monte_carlo_tests[i][j] == NULL) {
                exit(0);
            }
        }
    }

    // monte_carlo_tests[n-1][test_questions][0] = 2;
    // printf("test: %d\n", monte_carlo_tests[n-1][test_questions][0]);

    // Perform monte carlo simulation
    for ( j = 0; j < n; j++ ){
        int success = 1;
        while(success <= 12){
            // Randomly select question
            test[success] = Equilikely(1, 120);

            // Check if question has been chosen before
            if(test[success] != selected_questions[success]){
                selected_questions[success] = test[success];
                success++;
            }
        }

        // Add test case to monte carlo array
        for ( i = 0; i <= test_questions; i++ )
            monte_carlo_tests[j][i][0] = test[i];

        // Determine if each question is class 1 or class 2
        for ( i = 1; i <= test_questions; i++){
            if (test[i] <= class1_questions)
                monte_carlo_tests[j][i][1] = 1;
            else
                monte_carlo_tests[j][i][1] = 2;
        }

        // Clear out selected questions
        for ( i = 0; i <= total_questions; i++){
            selected_questions[i] = 0;
        }

        // Clear out test array
        for ( i = 0; i <= test_questions; i++){
            test[i] = 0;
        }
    }

    // Create an array to hold grade values for each monte carlo simulation
    float final_grades[n];
    for ( j = 0; j < n; j++)
        final_grades[j] = 0;

    // Find grade of each set of questions 
    int max_value = 0;
    for ( j = 0; j < n; j++ ){
        // Grading variables
        float grade_array[] = {0, 0, 0, 0, 0};
        int random_value = 0;

        // Debug only
        if ( DEBUG == 1 ){
            printf("Test Scores for N = %d (question, class):\t", j);
            for ( i = 1; i <= test_questions; i++ ){
                printf("(%d, #%d) ", monte_carlo_tests[j][i][0], monte_carlo_tests[j][i][1]);
            }
            printf("\n");
        }

        // For loop through each test question
        for ( i = 1; i <= test_questions; i++ ){
            // Determine grading scale based on question class
            if ( monte_carlo_tests[j][i][1] == 1){
                for ( k = 0; k < 5; k++ )
                    grade_array[k] = class1[k] * 10;
            }
            else{
                for ( k = 0; k < 5; k++ )
                    grade_array[k] = class2[k] * 10;
            }

            // Make grade range
            grade_array[0] = grade_array[0];
            if ( grade_array[0] == 10 )
                grade_array[1] = 0;
            else
                grade_array[1] = grade_array[1] + grade_array[0];
            if ( grade_array[1] == 10 )
                grade_array[2] = 0;
            else
                grade_array[2] = grade_array[2] + grade_array[1];
            if ( grade_array[2] == 10 )
                grade_array[3] = 0;
            else
                grade_array[3] = grade_array[3] + grade_array[2];
            if ( grade_array[3] == 10 )
                grade_array[4] = 0;
            else
                grade_array[4] = grade_array[4] + grade_array[3];

            // Find equilikely chance of number between 1 and 10 
            random_value = Equilikely(1, 10);
            if ( DEBUG == 1){
                printf("\t{random: %2.0d, class: %1.0d, range: (%2.0f, %2.0f, %2.0f, %2.0f, %2.0f)}", random_value, monte_carlo_tests[j][i][1], grade_array[0], grade_array[1], grade_array[2], grade_array[3], grade_array[4]);
            }

            // Determine random grade value based on distribution
            if ( random_value <= grade_array[0] ){
                final_grades[j] = grades[0] + final_grades[j];
                if ( DEBUG == 1)
                    printf("\tgrade: %3.2f\n", grades[0]);
            }
            for ( k = 1; k < 5; k++){
                if ( random_value <= grade_array[k] && random_value > grade_array[k-1]){
                    final_grades[j] = final_grades[j] + grades[k];
                    if ( DEBUG == 1)
                        printf("\tgrade: %3.2f\n", grades[k]);
                }
            }
        }

        // Update if test will pass or fail
        if ( final_grades[j] >= passing_score)
            pass[j] = 1;
        else
            pass[j] = 0;

        // Look if max score reached
        if ( max_value < (int) final_grades[j] )
            max_value = (int) final_grades[j];

        // Debug only
        if ( DEBUG == 1){
            printf("\tFinal Grade for N = %d:\t%3.0f", j, final_grades[j]);
            if ( pass[j] == 0 )
                printf("\t(fail)\n\n");
            else 
                printf("\t(pass)\n\n");
        }
    }

    // Initializes an empty array for the histogram
    float histogram[max_value + 1];
    for ( j = 0; j < (max_value + 1); j++)
        histogram[j] = 0;

    // Count frequency of each score
    for ( j = 0; j < n; j++){
        histogram[(int) final_grades[j]]++;
    }

    // Debug only
    if ( DEBUG == 1 ){
        printf("\nFrequency Histogram {score:frequency}:\t\t");
        for ( i = 1; i <= max_value; i++ ){
            if ( histogram[i] > 0 )
                printf("{%d:%3.2f}\t", i, (histogram[i]));
        }
        printf("\n\n");
    }
    

    // Calculate relative frequency of each score
    for ( j = 0; j < max_value + 1; j++){
        histogram[j] = histogram[j] / n;
    }

    printf("Results after %d iterations:\n", n);
    printf("\t...histogram {x:f^(x)}:\t\t");
    int beautify = 0;
    for ( i = 1; i <= max_value; i++ ){
        if ( histogram[i] > 0 ){
            printf("{%d:%3.5f}\t", i, (histogram[i]));
            beautify++;
            if ( beautify % 8 == 0){
                printf("\n\t\t\t\t\t");
            }
        }
    }
    printf("\n");

    // Calcualte probability of passing test
    float sum_pass = 0;
    for ( i = 0; i < n; i++ ){
        if ( pass[i] == 1 )
            sum_pass++;
    }
    float prob_pass = sum_pass / n;

    printf("\t...prob. of passing test:\t%3.2f%% \n\n", prob_pass * 100);

    // Deallocate memory
    for ( i = 0; i < n; i++){
        for ( j = 0; j <= test_questions; j++ )
            free(monte_carlo_tests[i][j]);
        free(monte_carlo_tests[i]);
    }
    free(monte_carlo_tests);

    // Return function
    return 0;
}
