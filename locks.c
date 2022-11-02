#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#define BILLION 1000000000L

//t - ticket lock grab number 
//r - ticket lock actual number 
//p - size of the array in queue based lock 
//k - time inside the critical section
//m - delay between critical sections 
//n - number of times it locks and unlocks 
//pid - processor number 



int
main(int argc, char **argv) {
    struct timespec start, end;

    if(argc != 2) {
        printf("Usage: lock n\nAborting...\n");
        exit(0);
    }

    n = atoi(argv[1]);

    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    time = BILLION *(end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec);
    time = time / BILLION;

    return 0;
} 

