/*    Finite Queue C-Program
 *    Author: Anna DeVries
 *    Date: 3 Mar 2021
 *
 *    Useage: gcc -c rng.c
 *            gcc -lm <file>.c rng.o -o <file> && ./<file>
 * 
 *    Description:  creates a finite queue (capacity determined at compile time)
 *                  tasks arrive with an exponential(2.0) distribution
 *                  a task's service time is determined with a uniform(1.0, 3.0) distribution
 *                  calculates steady-state statistics
 *          
 *                             _________   ______
 *                                      | /      \
 *             ------------->   queue   || server | -------------> 
 *             arrival event            ||        | completion event
 *                             _________| \______/ 
 */           

//  Libraries
#include <stdio.h>
#include <math.h>                                             
#include "rng.h"

//  Macros
#define LAST          100000L                   /* number of jobs processed */ 
#define START         0.0                       /* initial time             */ 
#define QUEUE_CAP     6                         /* queue capacity           */
#define DEBUG         0                         /* debug value              */
                 
//  Generate an Exponential random variate, use m > 0.0 
double Exponential(double m){                                       
  return (-m * log(1.0 - Random()));     
}
          
//  Generate a Uniform random variate, use a < b 
double Uniform(double a, double b){                                         
  return (a + (b - a) * Random());    
}

//  Generate the next arrival time
double GetArrival(void){       
  static double arrival = START;                                        
  arrival += Exponential(2.0);
  return (arrival);
}

//  Generate the next service time
double GetService(void){
  return (Uniform(1.0, 3.0));
}

//  Main Function
int main(void){
  //  Initialize variables
  long    index     = 0;                                /* job index            */
  double  arrival   = START;                            /* time of arrival      */
  double  delay;                                        /* delay in queue       */
  double  service;                                      /* service time         */
  double  wait;                                         /* delay + service      */
  double  departure = START;                            /* time of departure    */
  long    rejection = 0;                                /* rejection counter    */
  long    queue_count;                                  /* queue size counter   */
  double  queue_arr[QUEUE_CAP] = {0};                   /* queue of active tasks*/
  struct {                                              /* sum of ...           */
    double delay;                                       /*   delay times        */
    double wait;                                        /*   wait times         */
    double service;                                     /*   service times      */
    double interarrival;                                /*   interarrival times */
  } sum = {0.0, 0.0, 0.0};  

  //  Random Seed
  PutSeed(123456789);

  //  Begin Creating Tasks
  while (index < LAST){
    // Increase task counter by 1
    index++;

    //  Get information regarding task arrival and service time
    arrival      = GetArrival();
    service      = GetService();

    // Determine delay, wait and departure values
    if (arrival < departure)
      delay      = departure - arrival;                 /* delay in queue    */
    else
      delay      = 0.0;                                 /* no delay          */
    wait         = delay + service;
    departure    = arrival + wait;                      /* time of departure */

    // Increment total delay, wait and service time for steady-state characteristic values
    sum.delay   += delay;
    sum.wait    += wait;
    sum.service += service;

    // Initializes queue counter
    queue_count = 0;

    // Removes any tasks whose departure < new arrival
    for(int i = 0; i < QUEUE_CAP; i++){
      if(queue_arr[i] < arrival)
        queue_arr[i] = 0;
    }

    // Counts the number of active tasks in queue
    for(int i = 0; i < QUEUE_CAP; i++){
      if( queue_arr[i] != 0){
        queue_count++;
      }
    }

    // Finds if queue is full
    if(queue_count == QUEUE_CAP){
      rejection++;
    }
    else{
      int first_zero = 0;
      for(int i = 0; i < QUEUE_CAP; i++){
        if( queue_arr[i] == 0 && first_zero == 0){
          queue_arr[i] = arrival + delay;
          first_zero = 1;
        }
      }
    }

    // Debug only print statement
    if (DEBUG == 1){
      printf("job: %ld\tarrival: %6.2f\tdeparture: %6.2f\tdelay+arrival: %6.2f\trejection: %ld\tqueue:", index, arrival, departure, delay+arrival, rejection);
      for(int i = 0; i < QUEUE_CAP; i++)
        printf("\t%6.2f", queue_arr[i]);
      printf("\n\n");
    }
  
  }
  sum.interarrival = arrival - START;

  printf("\nfor %ld jobs with queue capacity of %d\n", index, QUEUE_CAP);
  printf("   average interarrival time = %6.2f\n", sum.interarrival / index);
  printf("   average wait ............ = %6.2f\n", sum.wait / index);
  printf("   average delay ........... = %6.2f\n", sum.delay / index);
  printf("   average service time .... = %6.2f\n", sum.service / index);
  printf("   average # in the node ... = %6.2f\n", sum.wait / departure);
  printf("   average # in the queue .. = %6.2f\n", sum.delay / departure);
  printf("   utilization ............. = %6.2f\n", sum.service / departure);
  printf("   rejection ............... = %6ld\n", rejection);
  printf("   probability of rejection. = %9.5f\n", ((double)rejection / (double)index));

  return (0);
}
