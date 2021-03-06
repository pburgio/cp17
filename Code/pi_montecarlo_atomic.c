#include <math.h>
#include <string.h>
#include <stdio.h>

#include <omp.h>
#define USE_OMP
#define N_THREADS 4

#define SECONDS 1000000000
#define _printf(...) printf(__VA_ARGS__)
//#define _printf(...)

#include <time.h>
#include <sys/time.h>
unsigned long long gettime(void)
{
    struct timespec t;
    int r;

    r = clock_gettime(CLOCK_MONOTONIC, &t);
    if (r < 0) {
        printf("Error to get time! (%i)\n", r);
        return -1;
    }

    return (unsigned long long) t.tv_sec * SECONDS + t.tv_nsec;
}

#include <stdlib.h>
float randNumGen(){
    int random_value = rand(); //Generate a random number
    float unit_random = random_value / (float) RAND_MAX; //make it between 0 and 1
    return unit_random;
}

int main()
{
    unsigned long long t_start, t_delta, counter = 0;
    float in_count = 0.0, tot_count = 0.0;
	unsigned long tot_iterations = 1000L;
	/* Declare lock var */
	omp_lock_t lock;
	/* Init the lock */
	omp_init_lock(&lock);

#ifdef USE_OMP
	tot_iterations /= N_THREADS;
#endif
	_printf("[get_pi] tot_iterations = %llu\n", tot_iterations); 

    t_start = gettime();
    
    srand ( time(NULL) ); // initiate random seed

#ifdef USE_OMP
	#pragma omp parallel num_threads(N_THREADS) private(counter) shared(in_count, tot_count) //shared(lock)
#endif
	{
#ifdef USE_OMP
		_printf("[Thread %d/%d]\n", omp_get_thread_num(), omp_get_num_threads());
#else
		_printf("OpenMP is disabled\n");
#endif
		for(counter=0;counter<tot_iterations;counter++){

			float x = randNumGen();
			float y = randNumGen();

			float result = sqrt((x*x) + (y*y));
			{
				if(result<1){
					// _printf("[Thread %d/%d] in_count is %f\n", omp_get_thread_num(), omp_get_num_threads(), in_count);
                    #pragma omp atomic
					  in_count +=1.0;         //check if the generated value is inside a unit circle
					// _printf("[Thread %d/%d]     Now in_count is %f\n", omp_get_thread_num(), omp_get_num_threads(), in_count);
				}
				#pragma omp atomic
				  tot_count++;
			} // end of critical sec.
		}
		
	} // barrier
	
	/* Destroy lock */
	omp_destroy_lock(&lock);

	// _printf("[Thread %d/%d] in_count is %f tot_count is %d\n", omp_get_thread_num(), omp_get_num_threads(), in_count, tot_count);
    float pi = 4 * in_count/tot_count;
	
    t_delta = gettime() - t_start;
    
    printf("pi :\t \t3.1415926535897932384626433832795028841971693993751058209\n");
    printf("get_pi: \t%.10f \ttime:\t%llu \tus\n", pi, t_delta/1000);

	return 0;
}
