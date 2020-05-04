#include<stdio.h>
#include"omp.h"
#include<stdlib.h>
#include<time.h>
#include <math.h>
int n=50;
double* x;
double* y;				
double a,b, appx_a, appx_b;
double err_std;

int Nthreads=10;

//************************find standart deviation for given data after calculating a and b ***********************************
float findstd(){
	int i;
	double std=0;
	
	#pragma omp parallel for shared(n,x,y) private(i) reduction(+:std)  	
	for(i=0;i<n;i++){
		std+= (appx_a*x[i]+appx_b-y[i])*(appx_a*x[i]+appx_b-y[i]);
	}

	std = sqrt(std/n);
	return std;
}


void FindParameters(){

	double cov=0.0;
	double mean_x=0.0, mean_y=0.0;
	double stdX=0.0;
    int i;
    
	#pragma omp parallel for shared(n,x,y) private(i) reduction(+:mean_x, mean_y) 
	for(i=0;i<n;i++){
	mean_x+=x[i];
	mean_y+=y[i];
		}
	mean_x/=n;
	mean_y/=n;

	#pragma omp parallel for shared(n,x,y) private(i) reduction(+:cov, stdX) 
	for(i=0;i<n;i++){
	cov+=(x[i]-mean_x)*(y[i]-mean_y);
	stdX+=(x[i]-mean_x)*(x[i]-mean_x);
		}

	appx_a=cov/stdX;
	appx_b=mean_y-appx_a*mean_x;

}


int main(){
	int i;
	x = (double *) malloc(n * sizeof(double));
	y=  (double *) malloc(n * sizeof(double));
	
	srand(time(NULL));
	
	a=2*(rand()%100)/50.0-1;      // param. a in the range [-1, 3]
	
	b=3*(rand()%1000)/500.0-2;    // param. b in the range [-2, 4]
	
	omp_set_num_threads(Nthreads);

	//********************************generate data****************************
	#pragma omp parallel for 
	for(i=0;i<n;i++){
		x[i] = (rand()%1000)/1000.0;                  // generating independent x vector in the range [0, 1]
		double noise = (2*(rand()%1000)/1000-1)/10.0; // generating noise in the range [-0.1, 0.1]
		y[i]= a*x[i]+b+noise;                         // generating dependent y vector
		}

	printf("No |      x       |      y       |\n");
	for(i=0;i<n;i++){
		printf("%i |     %.2f     |     %.2f     |\n", i, x[i], y[i]);
		}
	
	//*******************************run the algorithm*************************

	double time1 = omp_get_wtime();
	FindParameters();
	
	double time2 = omp_get_wtime() - time1;
	printf("Real a=%f\n",a);
	printf("Real b=%f\n",b);

	printf("Approx a=%f\n",appx_a);
	printf("Approx b=%f\n",appx_b);
    
        printf("Time of running: %f\n",time2);
    
    err_std=findstd();
	printf("Standard deviation (Error):\t%f\n", err_std);
        free(x);
	free(y);
	return 0;
}