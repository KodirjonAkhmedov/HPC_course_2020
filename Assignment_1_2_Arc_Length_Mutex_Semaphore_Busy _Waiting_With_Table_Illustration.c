#include <stdlib.h>//ko

#include <stdio.h>

#include <string.h>

#include <math.h>

#include <pthread.h>

#include <semaphore.h>

#include <sys/time.h>



	//function to get the timings

unsigned long get_time()

{

        struct timeval tv;

        gettimeofday(&tv, NULL);

        unsigned long ret = tv.tv_usec;

        ret /= 1000;

        ret += (tv.tv_sec * 1000);

        return ret;

}





//variables to store results of numerical integration

double mutex_res = 0;          // result of calculation using mutex method

double semaphore_res = 0;      // result of calculation using semaphore method

double busy_wait_res = 0;      // result of calculation using busy wait method



// global "flags"

pthread_mutex_t mutex;

sem_t semaphore;

int busy_wait_flag = 0;



// you may need this global variables, but you can make them inside main()

double a;                 // left point

double b;                 // right point

long n;                   // number of discretization points

double h;                 // distance between neighboring discretization points

int TOTAL_THREADS;



long n_div_TThreads;

int USED_TOTAL_THREADS;

double Sing_Time;

double Mute_Time;

double Sema_Time;

double Busy_Time;



short showPrint_or_Not=1;

//mathematical function that we calculate the arc length (declaration, define it yourselves)

double f(double x);



//function to calculate numerical derivative

double n_df(double x);



//arc_length on a single thread

double serial_arc_length();



//multithreaded arc_length rule using busy waiting

void* busy_wait_arc_length(void*);

void busy_wait_main();



//multithreaded arc_length using mutex

void* mutex_arc_length(void*);

void mutex_main();



//multrthreaded arc_length using semaphore

void* semaphore_arc_length(void*);

void semaphore_main();



int main( int argc, char *argv[] )

{

    a = atoi(argv[1]);

    b = atoi(argv[2]);

    n = atof(argv[3]);

    h = (b-a)/n;

    TOTAL_THREADS = atoi(argv[4]);



// Below we check whether the number of threads is bigger or not than the number of discretization points.

// if Yes, then there is no need to use more than "the number of discretization points" threads. In this case, 

// we calculate part of integral (formula for arc length) on the interval [xi, xi+h] with length h, where xi=a+i*h.

//So we set n_div_TThreads=1, which is equal to the coefficient at the h on the interval [xi, xi + n_div_TThreads*h];



    if (TOTAL_THREADS>n){

       n_div_TThreads=1;

       USED_TOTAL_THREADS=n;        

    }else

    {

       n_div_TThreads=n/TOTAL_THREADS;

       USED_TOTAL_THREADS=TOTAL_THREADS;

    }

    

    printf("TOTAL NUMBER OF THREADS: %d\n", TOTAL_THREADS);    

    printf("USED THREADS: %d\n", USED_TOTAL_THREADS);

    

    long start = get_time();

    double duration;

    double result = serial_arc_length(0, n);

    duration = (get_time() - start);

    printf("solution on a single thread: %f, time: %f milliseconds\n", result, duration);

    busy_wait_main();    

    mutex_main();

    semaphore_main();

    

// As graphical library does not work normally (i.e. it doesn't look "beautiful") in a console mode, 

//so I print (in a table form) the results of relative speedup of Busy waiting, Mutex 

//and Semaphore relatively single thread time:

    showPrint_or_Not=0;

    n = pow(10,8);

    h = (b-a)/n;

    start = get_time();

    result = serial_arc_length(0, n);

    Sing_Time = (get_time() - start);

    printf("\n");

    printf("----------Relative Speedup Analysys---------------");

    printf("\n");

    printf("Number of discretization points n=%i", n);

    printf("\n");

    printf("Time on a single thread: %f milliseconds\n", Sing_Time);

    printf("-------------------------------------------------------------------\n");

   for (int i=1;i<=5;i++)

   {

     busy_wait_flag = 0;

     USED_TOTAL_THREADS=pow(2,i);

     n_div_TThreads=n/USED_TOTAL_THREADS;

     busy_wait_main();    

     mutex_main();

     semaphore_main();

     printf("%i) ", i);

     printf("When number of Threads = %i \n", USED_TOTAL_THREADS);

     printf("    Relative speedup of Busy waiting: %.2f \n", Sing_Time/Busy_Time);

     printf("    Relative speedup of Mutex: %.2f \n", Sing_Time/Mute_Time);

     printf("    Relative speedup of Semaphore: %.2f \n", Sing_Time/Sema_Time);

     printf("-------------------------------------------------------------------\n");

   }

    

    return 0;

}



double f(double x)

{

    return (2*x);

}



double n_df(double x)

{

    return (f(x+h)-f(x-h))/(2*h);

}



double serial_arc_length(int start, int n_local)

{

    double integ_value=0;

    for (int i=start;i<=start+n_local-1;i++)

    {

        double xi=a+h*i;

        integ_value+=h/2*( sqrt(1+pow(n_df(xi),2)) +sqrt(1+pow(n_df(xi+h),2)) );

    }

    return integ_value;

}



void* busy_wait_arc_length(void* rank)

{

   int idx = (int)rank; 

   long Npoints;

   double temp_data;

   if (idx==USED_TOTAL_THREADS-1){ Npoints=n- idx*n_div_TThreads; }else{ Npoints=n_div_TThreads; }


  while(idx!=busy_wait_flag){

          } 

   temp_data =serial_arc_length(idx * n_div_TThreads, Npoints);

    busy_wait_res+= temp_data;

    busy_wait_flag++;

}



void* mutex_arc_length(void* rank)

{

   double temp_data;

   int idx = (int)rank; 

   long Npoints;

   if (idx==USED_TOTAL_THREADS-1){ Npoints=n- idx*n_div_TThreads; }else{ Npoints=n_div_TThreads; }

   temp_data =serial_arc_length(idx * n_div_TThreads, Npoints);

   pthread_mutex_lock(&mutex);

   mutex_res+= temp_data;

   pthread_mutex_unlock(&mutex);

}





void* semaphore_arc_length(void *rank)

{

   int idx = (int)rank; 

   long Npoints;

   double temp_data;

   if (idx==USED_TOTAL_THREADS-1){ Npoints=n- idx*n_div_TThreads; }else{ Npoints=n_div_TThreads; }

   temp_data =serial_arc_length(idx * n_div_TThreads, Npoints);
   sem_wait(&semaphore); 

   semaphore_res += temp_data;

   sem_post(&semaphore);

}



void busy_wait_main()

{

    pthread_t* thread_bus;

    thread_bus = malloc( USED_TOTAL_THREADS * sizeof(pthread_t));



    long start = get_time();

    double duration;



    for (int i = 0; i < USED_TOTAL_THREADS; i++) 

        pthread_create(&thread_bus[i], NULL, &busy_wait_arc_length, (void*)i); 

  

    for (int i = 0; i < USED_TOTAL_THREADS; i++) 

        pthread_join(thread_bus[i], NULL); 

        

    duration = (get_time() - start);

    Busy_Time=duration;

    if (showPrint_or_Not){

        printf("Solution using busy waiting: %f, time: %f milliseconds\n", busy_wait_res, duration);

    }



    free(thread_bus);    

}



void mutex_main()

{

    pthread_t* threads_mut;

    threads_mut = malloc( USED_TOTAL_THREADS * sizeof(pthread_t));



    long start = get_time();

    double duration;

    for (int i = 0; i < USED_TOTAL_THREADS; i++) 

        pthread_create(&threads_mut[i], NULL, &mutex_arc_length, (void*)i); 

  

    for (int i = 0; i < USED_TOTAL_THREADS; i++) 

        pthread_join(threads_mut[i], NULL); 



        duration = (get_time() - start);

    Mute_Time=duration;

    if (showPrint_or_Not){

        printf("Solution using mutex: %f, time: %f milliseconds\n", mutex_res, duration);

    }



    free(threads_mut);    

}



void semaphore_main()

{

    pthread_t* thread_Sem;

    thread_Sem = malloc( USED_TOTAL_THREADS * sizeof(pthread_t));



    long start = get_time();

    double duration;

    sem_init(&semaphore, 0, 1); 

    for (int i = 0; i < USED_TOTAL_THREADS; i++) 

        pthread_create(&thread_Sem[i], NULL, &semaphore_arc_length, (void*)i); 

  

    for (int i = 0; i < USED_TOTAL_THREADS; i++) 

        pthread_join(thread_Sem[i], NULL); 



    sem_destroy(&semaphore); 

    duration = (get_time() - start);

    Sema_Time=duration;

    if (showPrint_or_Not){

        printf("Solution using semaphore: %f, time: %f milliseconds\n", semaphore_res, duration);

    }



    free(thread_Sem);    

}


