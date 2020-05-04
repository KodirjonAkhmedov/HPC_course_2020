#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

 double 
f (double x) 
{
 return 4 / (1 + x * x);}

int 
main (int argc, char *argv[]) 
{
 
  int niter = 100000;	//number of iterations per FOR loop
  double x;		      	//x value for the random numer in [0, 1]
  double pi;			//holds approx value of pi
  int numthreads = 4;
  int i;

unsigned int seed = 123456789 * omp_get_thread_num ();
srand48 ((int) time (NULL) ^ omp_get_thread_num ());	//Give random() a seed value
double start_time = omp_get_wtime ();
double sum;
omp_set_num_threads (numthreads);
 
#pragma omp parallel for private(i, x) shared(niter) reduction(+:sum)
    for (i = 0; i <= niter; i++)
    {
double x = ((double) rand_r (&seed) / (RAND_MAX));
sum += f (x);} 
 
pi = 1.0 / niter * sum;

double run_time = omp_get_wtime () - start_time;

printf ("\n pi is %f in %f seconds %d threds \n ", pi, run_time, numthreads);

return 0; }


 
 
 
 
