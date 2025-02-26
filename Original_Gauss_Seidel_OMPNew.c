#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include<time.h>

/* method of Gauss-Seidel for the system of equantions A*x = b */

int n=7;           //the dimension of the system;
int numthreads=4;  //number of threads;

double **A;  //coefficients of Ax=b;
double *x;   // seeking solution of Ax=b
double *b;   //right-hand vector of Ax=b

double tol = 1.0e-4; //required accuracy;
int maxit = 1000;    //maximal number of iterations;
int numit;           //stopping step of iteration, when we reach the given accuracy;

void run_gauss_seidel_method();

int main ( int argc, char *argv[] )
{
   int i,j;
   
   srand(time(NULL));
   
   omp_set_num_threads(numthreads);
   
   b = (double*) calloc(n,sizeof(double));
      
   A = (double**) calloc(n,sizeof(double*));
   for(i=0; i<n; i++)
            A[i] = (double*) calloc(n,sizeof(double));
            
   // generating A matrix (A is diagonally dominant) and b right-hand vector
   for(i=0; i<n; i++)
   {
      b[i] = rand() % 20;
      for(j=0; j<n; j++) A[i][j] = 1+rand()% 10;
      A[i][i] = n*10+ rand() % 5;
   }
   
   printf("System of equation:\n");
      for(i=0; i<n; i++)
   {
      for(j=0; j<n; j++) printf(" + %2.f*x%i ", A[i][j], j+1);
      printf(" = %2.f\n", b[i]);
   }
   
    x = (double*) calloc(n,sizeof(double));  /* x[i]=0 initial approx. of solution at 0-iteration*/
    
    
    run_gauss_seidel_method();  
        
   printf("It is runned %d iterations to reach an approximate solution of above system for given tolerance\n",numit);
   printf("Approximate solution is \n");
   for(i=0; i<n; i++) 
   printf("x%i=%.3f\n", i+1, x[i]);
   
   printf("For above obtained approximate solution:\n");
   
      for(i=0; i<n; i++)
   {  double sum=0.0;
      for(j=0; j<n; j++) {
          printf(" +%2.f*x%i ", A[i][j], j+1);
          sum+=A[i][j]*x[j];
      }
          
      printf(" apprx= %2.f  ( exact right-hand %2.f )\n", sum, b[i]);
   } 
 
   return 0;
}


void run_gauss_seidel_method ()
{
   double *delta_x;
   delta_x = (double*) calloc(n,sizeof(double));
   
   int i,j,k;
   double delxi;

   for(k=0; k<maxit; k++)
   {
      double norm_error = 0.0;
      for(i=0; i<n; i++)
      {
         delta_x[i] = b[i];
         delxi = 0.0;
         
         #pragma omp parallel for shared(A,x) private(j) reduction(+:delxi)
            for(j=0; j<n; j++) delxi += A[i][j]*x[j];
            
         delta_x[i] -= delxi;
         delta_x[i] /= A[i][i];
         x[i] += delta_x[i];
         norm_error += delta_x[i]*delta_x[i];
      }
      if(sqrt(norm_error) <= tol) break;
   }
   numit = k+1;
   free(delta_x);
}