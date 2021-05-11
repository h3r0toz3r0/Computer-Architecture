/*
 * Title: Continuous-Data Histogram for Service Times
 * Author: Anna DeVries
 * Date: 22 March 2021
 *
 * Description:     generates a continuous data histogram from file ac.dat.
 *                  The file consists of data points <arrival time, departure time>.
 *                  First, the program finds the service time of each task, and then
 *                  creates a continous data histgram. Next, the program calculates the
 *                  mean and standard deviation based on the histogram. Finally, the program
 *                  calculates the mean and utilization of the sample.
 * 
 * Useage:      gcc -c rng.c
 *              gcc -lm <file>.c rng.o -o <file>
*/

//  Libraries
#include <stdio.h>
#include <math.h> 

//  Macros
#define  MIN     0.0
#define  MAX     16.0                               /* select to min outliers       */
#define  K       18                                 /* number of histogram bins     */
                                                    /* log2(n) < k < sqrt(n)        */
                                                    /* 9 < k < 22                   */
#define  DELTA   ((MAX - MIN) / K)                  /* histogram bin size (width)   */
#define  sqr(x)  ((x) * (x))                        /* calculate square             */
#define FILENAME   "ac.dat"                         /* data file name               */
#define START      0.0                              /* start time of data           */
#define DEBUG      0                                /* debug value                  */

//  Get arrival time of task   
double GetArrival(FILE *fp){ 
    double a;
    fscanf(fp, "%lf", &a);
    return (a);
}

//  Get departure time of task
double GetDeparture(FILE *fp){ 
    double s;
    fscanf(fp, "%lf\n", &s);
    return (s);
}

//  Main Function
int main(void){
    // Local Variables
    FILE *fp;                                       /* file pointer                                 */
    double x;                                       /* data value                                   */
    long   j;                                       /* histogram bin index                          */
    long   index    = 0;                            /* sample size                                  */
    long   count[K] = {0.0};                        /* bin count                                    */
    double midpoint[K];                             /* bin midpoint                                 */
    struct {                                       
        long lo;
        long hi;
    } outliers      = {0, 0};                       /* outliers low and high                        */
    double sum      = 0.0;                          /* sum of values                                */
    double sumsqr   = 0.0;                          /* sum squared                                  */
    double mean;                                    /* histogram mean                               */
    double stdev;                                   /* histogram standard dev                       */
    double d = 0;
    double x_mean = 0;
    double v = 0;
    long   job_index     = 0;                       /* job index                                    */
    double an   = START;                            /* arrival time                                 */
    double dn = 0;                                  /* delay                                        */
    double bn = 0;                                  /* time service begins = arrival + delay        */
    double wn = 0;                                  /* wait = delay + service                       */
    double sn = 0;                                  /* service time                                 */
    double cn = START;                              /* departure time = arrival + wait              */
    double last_cn = START;                         /* previous departure time (cn-1)               */
    struct {                                        /* sum of ...                                   */
        double delay;                               /*   delay times                                */
        double wait;                                /*   wait times                                 */
        double service;                             /*   service times                              */
        double interarrival;                        /*   interarrival times */
        double arrival;                             /*   interarrival times */
    } summation = {0.0, 0.0, 0.0};      

    // Intro
    printf("\nContinous Data Histogram Problem\n");
    printf("Author: Anna DeVries\n");
    printf("Date: 22 March 2021\n");
    printf("Description:\tGiven a file ac.dat of <arrival_time departure_time>.\n");
    printf("\t\tThis program converts data to service time assuming single-server queue.\n");
    printf("\t\tFor the continous data histogram: a = %3.0f, b = %3.0f, k = %3.0f.\n", MIN, MAX, (float) K);
    printf("Output: continous-data histogram, and histogram mean and standard deviation\n");
    printf("---------------------------------------------------------------------------------------\n\n");                  

    // Initialize midpoint of histogram
    for (j = 0; j < K; j++) 
        midpoint[j] = MIN + (j + 0.5) * DELTA;    

    //  Open file, quit on error
    fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open input file %s\n", FILENAME);
        return (1);
    }

    //  While reading file
    while (!feof(fp)) {
        //  Update index
        job_index++;

        //  Get arrival and departure times
        an      = GetArrival(fp);
        cn      = GetDeparture(fp);

        //  Calculate wait time
        wn = cn - an;

        //  Find delay time
        if ( last_cn > an ){
            dn = last_cn - an;
        }
        else{
            dn = 0;
        }

        //  Calculate time service begins
        bn = an + dn;

        //  Find service time
        sn = wn - dn;

        //  Set last cn
        last_cn = cn;

        //  Add to index value for histogram
        index++;

        //  Determine if service time is an outlier
        x = sn;
        if ((x >= MIN) && (x < MAX)) {
            j = (long) ((x - MIN) / DELTA);
            count[j]++;
        }
        else if (x < MIN)
            outliers.lo++;
        else
            outliers.hi++;

        //  Troubleshooting
        if (DEBUG == 1){
            printf("\ttask: %ld\t\t\tan=%6.2f    bn=%6.2f    cn=%6.2f    ::  wn=%6.2f    dn=%6.2f    sn=%6.2f\n", job_index, an, bn, cn, wn, dn, sn);
        }

        //  Welford's One Pass Algorithm
        d = sn - x_mean;
        v = v + d * d * (index - 1) / index;
        x_mean = x_mean + d / index;

        //  Update sum values
        summation.delay   += dn;
        summation.wait    += wn;
        summation.service += sn;
        summation.arrival += an;
    }
    summation.interarrival = an - START;

    //  Calculate Histogram Mean
    for (j = 0; j < K; j++)                     
        sum += midpoint[j] * count[j];
    mean   = sum / index;   

    //  Calculate Histogram Standard Deviation
    for (j = 0; j < K; j++)                     
        sumsqr += sqr(midpoint[j] - mean) * count[j];
    stdev     = sqrt(sumsqr / index);   

    //  Print the Histogram
    printf("\nContinious-Data Histogram:\n\n");
    printf("\t  bin     midpoint     count   proportion    density\n");
    for (j = 0; j < K; j++) { 
        printf("\t%5ld", j + 1);                                    /* bin        */
        printf("%12.3f", midpoint[j]);                              /* midpoint   */
        printf("%10ld", count[j]);                                  /* count      */
        printf("%12.3f", (double) count[j] / index);                /* proportion */
        printf("%12.3f\n", (double) count[j] / (index * DELTA));    /* density    */
    }
    printf("\n\tsample size .... = %7ld\n", index);
    printf("\tmean ........... = %7.3f\n", mean);
    printf("\tstdev .......... = %7.3f\n\n", stdev);
    if (outliers.lo > 0)
        printf("\tNOTE: there were %ld low outliers\n", outliers.lo);
    if (outliers.hi > 0)
        printf("\tNOTE: there were %ld high outliers\n", outliers.hi);  

    //  Print Server Information
    printf("---------------------------------------------------------------------------------------\n");
    printf("\nSingle Server Queue Information:\n");
    printf("\n\tsample size .... = %7ld\n", job_index);
    printf("\tmean ........... = %7.3f\n", summation.service / job_index);
    printf("\tstdev .......... = %7.3f\n", sqrt(v / index));
    printf("\tutilization .... = %7.3f\n\n", summation.service / cn);
    printf("\tNOTE: this is sample information, it will differ from continious-data histogram.\n\n");                             

    // Close file
    fclose(fp);

    // Return after successful completition
    return (0);
}
