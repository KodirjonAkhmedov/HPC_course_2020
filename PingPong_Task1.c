#include <mpi.h>   
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {

  static const int MAX_MULTIPLYER = 10; // ping pong limit
  static const int BYTES = 1024;
  static const int MESSAGE_COUNT = 100;

  int my_rank;
  int size;
  char hostname[100];
  char debug[100];
  double start_t, end_t, delta_t;
  double start_total_time, end_total_time, delta_total;
  int i, j;
  MPI_Status stat;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
  MPI_Comm_size(MPI_COMM_WORLD, &size); 
  gethostname(hostname, 100);

  // We are assuming at least 2 processes for this task

  if (size != 2) {
    fprintf(stderr, "World size must be two for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  
	MPI_Barrier(MPI_COMM_WORLD);

  int iteration = 0;	
  start_total_time = MPI_Wtime(); // Starting the whole time 
  
  for(j = 1; j <= MAX_MULTIPLYER; j++)
  {
    iteration++;
    int message_size = BYTES * j;
    char message[message_size]; // chars are 8 bits each or 1 byte 
    
    if (my_rank == 0)
    { 
      
      for (i=0; i < message_size - 1; i++)
      {
         message[i] = 'A' + random() % 2;
      }
      message[message_size] = (char)0;
      printf("Iteration number: %d\n", iteration);
      sprintf(debug, "message of %d bytes is:\n%s", message_size, "");
      fputs(debug, stdout);

      // time is starting here
      start_t = MPI_Wtime();
      for (i = 0; i < MESSAGE_COUNT; i++)
      { 
        // send message; 
        //sprintf(debug, "rank 0 sending message %s\n", "");
        //fputs(debug, stdout);
        MPI_Ssend(message, strlen(message)+1, MPI_BYTE, 1, i, MPI_COMM_WORLD);
        // receive message;
        MPI_Recv(message, sizeof(message), MPI_BYTE, 1, i, MPI_COMM_WORLD, &stat);
         //sprintf(debug, "rank 0 received message %s\n", "");
         //fputs(debug, stdout);
      } 
      // stop timer;
      end_t = MPI_Wtime();
      delta_t = end_t - start_t;
      sprintf(debug, "%d, %f\n", message_size, (delta_t / (2*MESSAGE_COUNT)));
      fputs(debug, stdout);
    } 
    else // rank 1
    { 
      // sprintf(debug, "rank 1 is alive\n");
      // fputs(debug, stdout);
      for (i = 0; i < MESSAGE_COUNT; i++)
      { 
        // receive message;
        MPI_Recv(message, sizeof(message),MPI_BYTE, 0, i, MPI_COMM_WORLD, &stat);
         //sprintf(debug, "rank 1 received message %s\n", "");
         //fputs(debug, stdout);
        // send message;
         //sprintf(debug, "rank 1 sending message %s\n", "");
         //fputs(debug, stdout);
        MPI_Ssend( message, strlen(message)+1, MPI_BYTE, 0, i, MPI_COMM_WORLD);
      } 
    }
  }
  
  MPI_Finalize();
  
  end_total_time = MPI_Wtime();
  delta_total = end_total_time - start_total_time;
  printf("Total time spent for each processes 0|1 %f \n", delta_total);

  return 0;
}
