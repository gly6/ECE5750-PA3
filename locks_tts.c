#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
//#define BILLION 1000000000

//t - ticket lock grab number 
//r - ticket lock actual number 
//p - size of the array in queue based lock 
//k - time inside the critical section
//m - delay between critical sections 
//n - number of times it locks and unlocks 
//pid - processor number 

typedef struct lock_tts{
    int lock_data; 
};

typedef struct threads_lock {
    int pid, n, k, m; 
    int *a; 
    double *time_array;
    struct lock_tts *lkp;
} threads_lock_t;


void tts_lock(struct lock_tts *lkp) {
    s_lock(lkp); 
}

void tts_unlock(struct lock_tts *lkp){
    s_unlock(lkp);
}

//record_timestamp(pid)
//for i=0 to N-1
//    lock_acquire(lock)
//    for j=0 to k-1
//        q++
//    endfor
//    lock_release(lock)
//    for j=0 to m-1
//        p++
//    endfor
//endfor
//record_timestamp(pid)
//a[pid] = p+q

void * tts(void *varg){
    threads_lock_t *arg = varg;
    double time;
    struct timespec start, end; 
    int pid, n, k, m; 
    int *a;
    double *time_array;
    struct lock_tts *lkp;
    pid = arg->pid; 
    n = arg->n; 
    k = arg->k; 
    m = arg->m; 
    a = arg->a;
    time_array = arg->time_array; 
    lkp = arg->lkp;
    int p = 0; 
    int q = 0; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < n-1; i++){
        tts_lock(lkp); 
        for (int j = 0; j < k-1; j++){
            q++;
        }
        tts_unlock(lkp);
        for (int j = 0; j < m-1; j++){
            p++;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end); 
    time = 1.0e9 *(end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec);
    time = time / 1.0e9;
    time_array[pid] = time; 
    a[pid] = p+q; 
    return NULL; 
}

int
main(int argc, char **argv) {
    int p, n, k, m;
    double *time_array;
    int *a;
    struct lock_tts *lkp = (struct lock_tts*)malloc(sizeof(struct lock_tts));
    

    if(argc != 5) {
        printf("Usage: lock p n k m\nAborting...\n");
        exit(0);
    }

    p = atoi(argv[1]);
    n = atoi(argv[2]);
    k = atoi(argv[3]);
    m = atoi(argv[4]); 

    s_lock_init(lkp);
    
    time_array = (double *) malloc (p* sizeof(double));
    a = (int *)malloc(p* sizeof(int));
    
    pthread_t *threads = malloc(p * sizeof(threads));
    for(int i = 0; i < p; i++) {
        threads_lock_t *arg = malloc(sizeof(*arg));
        arg->pid = i;
        arg->n = n;
        arg->k = k; 
        arg->m = m;
        arg->time_array = time_array;
        arg->a = a;
        arg->lkp = lkp;
        pthread_create(&threads[i], NULL, tts, arg);
    }
    for(int i = 0; i < p; i++)
        pthread_join(threads[i], NULL);


    free(threads);

    double max_time = 0;
    int max_proc = 0;
    for (int i = 1; i < p; i++) {
        if (max_time < time_array[i]){
            max_time = time_array[i];
            max_proc = i;
        }
    }

    for(int i = 0; i < p; i++){
        printf("PID: %d, Elapsed time: %f seconds, Test number: %d\n", i,  time_array[i], a[i]);
    }
    
    printf("PID: %d, Max Time: %f seconds", max_proc, max_time);
    
    return 0;
} 

