#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int Nthreads = 10;
int N = 1000;			// order of square NxN matrix
double **A, **B, **C;		// matrices; C=A*B

double ** Calloc_matrix()
{
    double ** matrix =  (double **) calloc (N, sizeof (double *));
    
    for (int i = 0; i < N; i++) matrix[i] = (double *) calloc (N, sizeof (double));
   
    return matrix;
}

void generateRndMatrix (double **matrix)
{
 
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)  matrix[i][j] = (rand ()%1000) / 1000.0;

}

void Free_Matx (double **matrix)
{
  for (int i = 0; i < N; ++i) free (matrix[i]);
  free (matrix);
}

int
main ()
{

  int i, j, n;

  omp_set_num_threads (Nthreads);
  
  A=Calloc_matrix();
  B=Calloc_matrix();
  C=Calloc_matrix();
  
  srand (time (NULL));
  generateRndMatrix (A);
  generateRndMatrix (B);
  
  //----------------------------------------------         
  printf ("First part:\n");
  double timeSt = omp_get_wtime ();
  
#pragma omp parallel for schedule(static) collapse(2) private(i,j,n) shared(A,B,C)
    for (i = 0; i < N; i++)
      for (j = 0; j < N; j++)
	    for (n = 0; n < N; n++) C[i][j] += A[i][n] * B[n][j];
  
  double Runduration = omp_get_wtime () - timeSt;
  printf ("Time of execution (ijn): %f seconds.\n", Runduration);
  //----------------------------------------------
   
   C=Calloc_matrix();
   printf ("Secord part:\n");
   timeSt = omp_get_wtime ();

#pragma omp parallel for schedule(static) collapse(2) private(i,j,n) shared(A,B,C)
    for (j = 0; j < N; j++)
      for (i = 0; i < N; i++)
	    for (n = 0; n < N; n++) C[i][j] += A[i][n] * B[n][j];

  Runduration = omp_get_wtime () - timeSt;
  printf ("Time of execution (jin): %f seconds.\n", Runduration);
  //----------------------------------------------
  
    C=Calloc_matrix();
    printf ("Third part:\n");
    timeSt = omp_get_wtime ();
#pragma omp parallel for schedule(static) collapse(2) private(i,j,n) shared(A,B,C)
    for (n = 0; n < N; n++)
      for (i = 0; i < N; i++)
	    for (j = 0; j < N; j++) C[i][j] += A[i][n] * B[n][j];
	    
  Runduration = omp_get_wtime () - timeSt;
  printf ("Time of execution (nij): %f seconds.\n", Runduration);
  //----------------------------------------------
  
  Free_Matx (A);
  Free_Matx (B);
  Free_Matx (C);

  return 0;
}
