#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

typedef struct lock_tts{
    int lock_data; 
};

int current_serving = 0; 
int next_available = 0;



typedef struct threads_lock {
    int pid, n, k, m; 
    int *a; 
    double *time_array;
    struct lock_tts *lkp;
} threads_lock_t;



void ticket_lock(struct lock_ *lkp) {    
    s_lock(lkp); 
    int my_ticket = next_available; 
    next_available++;
    s_unlock(lkp);
    while(my_ticket > current_serving);
}

void ticket_unlock(){
    current_serving++;
}

void * ticket(void *varg){
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
        ticket_lock(lkp); 
        for (int j = 0; j < k-1; j++){
            q++;
        }
        ticket_unlock();
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
        printf("Usage: lock p k m n\nAborting...\n");
        exit(0);
    }

    p = atoi(argv[1]);
    k = atoi(argv[2]);
    m = atoi(argv[3]); 
    n = atoi(argv[4]);
    //n = 20000000;

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
        pthread_create(&threads[i], NULL, ticket, arg);
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