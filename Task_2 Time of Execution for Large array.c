#include <stdio.h>
#include<stdlib.h>
#include "mpi.h"
#include<time.h>
#define MASTER  0

int main()
{
    int *rule, *Trule;
    int *a, *ghost, *Tghost;
    int i, Arr_size;
    int id, Nproc;

    int p2,p1,p0;

    int niter, GRID_HEIGHT;
    
    int chunksize, chunksizeRight;
    int *send_count,*Tsend_count;
    int *disp, *Tdisp;  
    int interRule;

    //Large one dimensional cellular automata array (ODCAA) size
    Arr_size=100000; 
    
    //Large level, Number of iteration, i.e number of levels
    GRID_HEIGHT=10000;
    
    a = (int *) malloc((Arr_size+2) * sizeof(int));
    rule = (int *) malloc(8 * sizeof(int));
    Trule = (int *) malloc(8 * sizeof(int));

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &Nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    
    send_count = (int *) malloc(Nproc * sizeof(int));   
    Tsend_count = (int *) malloc(Nproc * sizeof(int));   
    disp= (int *) malloc(Nproc * sizeof(int));
    Tdisp= (int *) malloc(Nproc * sizeof(int));
    
    //chunksize from id=0 till id=Nproc-2 processors 
    chunksize = Arr_size / Nproc;
    
    //chunksize for the id=Nproc-1 processor
    //i.e chunksize for the last right part of ODCAA 
    chunksizeRight= chunksize+Arr_size % Nproc;
    
    // Array to receive part of one ODCAA
    ghost = (int *) malloc((chunksizeRight+2) * sizeof(int));
    
    // Array to send part of next level of ODCAA
    Tghost = (int *) malloc((chunksizeRight) * sizeof(int));

if (id==MASTER)
{   
    srand(time(NULL));
    //random rule
    printf("Rule:\n");
    for (i=0;i<=7;i++) 
    { 
        rule[i]=rand() %2;
        p0=i%2;
        p1=(i/2)%2;
        p2=i/4;
        printf("%i%i%i-->%i\n", p2,p1,p0, rule[i]);
    }

    //Constant boundary. Here use a[0] for left, a[Arr_size+1] for right conditions.
    a[0]=rand()%2; a[Arr_size+1]=rand()%2; 
    
    //generating random initial ODCAA
    for (i=1; i<=Arr_size; i++) {a[i]= rand()%2;}

     disp[0]=0; Tdisp[0]=1;
   for(int i=0; i<Nproc-1; i++) {
       send_count[i]=chunksize+2; 
       Tsend_count[i]=chunksize; 
       disp[i+1]=(i+1)*chunksize;
       Tdisp[i+1]=disp[i+1]+1;
   }
   send_count[Nproc-1]=chunksizeRight+2;
   Tsend_count[Nproc-1]=chunksizeRight;
   
}

double start = MPI_Wtime();
niter=0; 

  while (niter < GRID_HEIGHT) 
 {
  niter++; 

// We don't need to print results of levels


// Here we set recvcount size depending on processors id.
int recvcount = (id == Nproc-1) ? chunksizeRight+2 : chunksize+2;

//Here we bcast rule for all processors
MPI_Bcast(rule, 8, MPI_INT, MASTER, MPI_COMM_WORLD);

//Here we send parts of ODCA Array to all processors, including their boundaries
MPI_Scatterv(a, send_count, disp, MPI_INT, ghost, recvcount, MPI_INT, MASTER, MPI_COMM_WORLD);

//Here we find a new level values for ODCA Array, using rule.
for(int i=0; i<=recvcount-3;i++) Tghost[i]=rule[4*ghost[i]+2*ghost[i+1]+ghost[i+2]];

int Trecvcount = (id == Nproc-1) ? chunksizeRight : chunksize;
//Here we get a new level values from Tghosts and gather they to a new level values of ODCA Array
MPI_Gatherv(Tghost,Trecvcount, MPI_INT, a, Tsend_count, Tdisp, MPI_INT, MASTER, MPI_COMM_WORLD);

}

 double end = MPI_Wtime();
 if (id==MASTER) {
printf("Time of execution: %f\n with processors %d.", end-start, Nproc);
free(a); free(ghost); free(Tghost); free(rule); 
 }
   MPI_Finalize();
    return 0;
}
