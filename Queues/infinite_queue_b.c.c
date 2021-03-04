/*    Infinite Queue C-Program
 *    Author: Anna DeVries
 *    Date: 2 Mar 2021
 * 
 *    Description:  creates a infinite queue
 *                  tasks arrive with an exponential(2.0) distribution
 *                  service time occurs once a certain number of tasks arrive (geometric(0.9)) and each task is serviced with a time determined by uniform(0.1, 0.2)
 *                  calculates steady-state statistics
 *          
 *                             _________   ______
 *                                      | /      \
 *             ------------->   queue   || server | -------------> 
 *             arrival event            ||        | completion event
 *                             _________| \______/ 
 */   

#include <stdio.h>
#include <math.h>                                             
#include "rng.h"

#define LAST          100000L                   /* number of jobs processed */ 
#define START         0.0                       /* initial time             */ 
#define DEBUG         1


   double Exponential(double m)                 
/* ---------------------------------------------------
 * generate an Exponential random variate, use m > 0.0 
 * ---------------------------------------------------
 */
{                                       
  return (-m * log(1.0 - Random()));     
}


   double Uniform(double a, double b)           
/* --------------------------------------------
 * generate a Uniform random variate, use a < b 
 * --------------------------------------------
 */
{                                         
  return (a + (b - a) * Random());    
}

  long Geometric(double p)   
/* -------------------------------------------------------
 * generate a Geometric random variate, use 0.0 < p < 1.0
 * -------------------------------------------------------
 */
{
  return ((long) (log(1.0 - Random()) / log(p)));
}

   double GetArrival(void)
/* ------------------------------
 * generate the next arrival time
 * ------------------------------
 */ 
{       
  static double arrival = START;                                        

  arrival += Exponential(2.0);
  return (arrival);
}


   double GetService(void)
/* ------------------------------
 * generate the next service time
 * ------------------------------
 */ 
{
  long k;
  double sum = 0.0;
  long tasks = 1 + Geometric(0.9);
  for (k = 0; k < tasks; k++)
    sum += Uniform(0.1, 0.2);
  return (sum);
}


  int main(void)
{
  long   index     = 0;                         /* job index            */
  double arrival   = START;                     /* time of arrival      */
  double delay;                                 /* delay in queue       */
  double service;                               /* service time         */
  double wait;                                  /* delay + service      */
  double departure = START;                     /* time of departure    */

  struct {                                      /* sum of ...           */
    double delay;                               /*   delay times        */
    double wait;                                /*   wait times         */
    double service;                             /*   service times      */
    double interarrival;                        /*   interarrival times */
  } sum = {0.0, 0.0, 0.0};  

  int seed_arr[] = {12345};    // {123456789, -123456789, 12345};
  if(DEBUG == 0)
    printf("jobs, seed, average interarrival time, average wait, average delay, average service time, average number of tasks in node, average number of tasks in queue, utilization\n");

  for(int i = 0; i < 1; i++){
    PutSeed(seed_arr[i]);
    index = 0;

    while (index < LAST) {
      index++;
      arrival      = GetArrival();
      if (arrival < departure)
        delay      = departure - arrival;         /* delay in queue    */
      else
        delay      = 0.0;                         /* no delay          */
      service      = GetService();
      wait         = delay + service;
      departure    = arrival + wait;              /* time of departure */
      sum.delay   += delay;
      sum.wait    += wait;
      sum.service += service;

      if(DEBUG == 0){
          printf("%ld, %d, ", index, seed_arr[i]);
          printf("%6.2f, ", (arrival - START) / index);
          printf("%6.2f, ", sum.wait / index);
          printf("%6.2f, ", sum.delay / index);
          printf("%6.2f, ", sum.service / index);
          printf("%6.2f, ", sum.wait / departure);
          printf("%6.2f, ", sum.delay / departure);
          printf("%6.2f\n", sum.service / departure);
      }
    }
    sum.interarrival = arrival - START;

    if(DEBUG == 1){
      printf("\nfor %ld jobs and seed %d\n", index, seed_arr[i]);
      printf("   average interarrival time = %6.2f\n", sum.interarrival / index);
      printf("   average wait ............ = %6.2f\n", sum.wait / index);
      printf("   average delay ........... = %6.2f\n", sum.delay / index);
      printf("   average service time .... = %6.2f\n", sum.service / index);
      printf("   average # in the node ... = %6.2f\n", sum.wait / departure);
      printf("   average # in the queue .. = %6.2f\n", sum.delay / departure);
      printf("   utilization ............. = %6.2f\n", sum.service / departure);
    }
  }

  return (0);
}
