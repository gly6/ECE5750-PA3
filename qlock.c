
typedef struct {
    int * lock;
    int index;
    int size;
    int serving;
} qlock;

void q_lock_init(qlock * lk, int n)
{
    lk->lock = (int *) malloc(n*64*sizeof(int));
    for(int i = 0; i < n; i++)
        lk->lock[64*i] = 0;
    lk->size = n;
    lk->index =  0;
    lk->lock[0] = 1;
}

void q_lock(qlock * lk)
{
    int my_index = (int)(__atomic_fetch_add(&lk->index,1,__ATOMIC_RELAXED)) ;
    printf("%d",my_index);
    while(lk->lock[my_index%lk->size*64] == 0);
    lk->serving = my_index;
}

void q_unlock(qlock * lk)
{
    lk->lock[(int)((lk->serving+1)%lk->size)*64]=1; 
    lk->lock[lk->serving*64] = 0;
    
}

