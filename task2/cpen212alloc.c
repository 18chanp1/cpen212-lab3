#include <stdlib.h>
#include <string.h>
#include "cpen212alloc.h"
#include <unistd.h>>

typedef struct {
    void *end;   // end of heap
    void *free;  // next free address on heap
    void *head; //head of the heap
} alloc_state_t;

void *cpen212_init(void *heap_start, void *heap_end) {
    alloc_state_t *s = (alloc_state_t *) malloc(sizeof(alloc_state_t));
    s->end = heap_end;
    s->free = heap_start;
    s->head = heap_start;


    //header block
    *(unsigned long long int *)s->head = heap_end - heap_start;  // store int value 10 at address 0x16
    
    return s;
}

void cpen212_deinit(void *s) {
    free(s);
}

void *cpen212_alloc(void *alloc_state, size_t nbytes) {
    alloc_state_t *s = (alloc_state_t *) alloc_state;
    size_t aligned_sz = (nbytes + 7) & ~7;

    unsigned long long int* currentBlock = s->head;

    printf ("Head: %llu \n", currentBlock);

    while((*currentBlock & 1 || *currentBlock < aligned_sz + 8) && (currentBlock + (*currentBlock) < s->end)){
        //sleep(1);
        printf("Traverse Block: %llu \n", currentBlock);
        printf("Ta Block: %llu \n", *currentBlock);
        printf("Tail: %llu \n", s->end);
        
        if(*currentBlock & 1){
            currentBlock = currentBlock + ((*currentBlock) - 1);
        } else {
            currentBlock = currentBlock + (*currentBlock);
        }
        
        
    }

    printf ("nullchk, %llu \n", *currentBlock);
    if(currentBlock + aligned_sz + 8 > s ->end 
        || nbytes < 0 || *currentBlock & 1 || *currentBlock < aligned_sz + 8){
        
        return NULL;
    }

    unsigned long long int size = *currentBlock;
    //size *=8;

    printf ("Assigned Block: %llu \n", currentBlock);


    if (*currentBlock == aligned_sz + 8){
        (*currentBlock)++;
        return currentBlock + 8;
    }
    if (*currentBlock > aligned_sz + 8) {
        unsigned long long *splitBlock = currentBlock + 8 + aligned_sz;
        *(unsigned long long int *)splitBlock = *currentBlock - aligned_sz - 8;

        *(unsigned long long int *)currentBlock = aligned_sz + 1 + 8;

        return currentBlock + 8; //incremented
    }

    // unsigned long long int *nxtBlock = currentBlock + aligned_sz + 8;

    // *(unsigned long long int *)nxtBlock = size - aligned_sz - 8;

    // *(unsigned long long int *)currentBlock = aligned_sz + 8 + 1;


    // return currentBlock + 8;


    // void *p = s->free;
    // if ((p >= s->end) || (p + aligned_sz > s->end))
    //     return NULL;
    // s->free += aligned_sz;
    // return currentBlock + 8;
}

void cpen212_free(void *alloc_state, void *p) {
    printf("p: %llu \n", p);
    void *ptr = p - 64;
    unsigned long long int *currentblock = (unsigned long long int *) ptr;
    *(unsigned long long int *) currentblock = *currentblock - 1;
    printf("End of free: %llu @ %llu \n", *currentblock, currentblock);
 }

 

void *cpen212_realloc(void *alloc_state, void *prev, size_t nbytes) {
    //get header
    size_t aligned_sz = (nbytes + 7) & ~7;
    unsigned long long* oldBlock = prev-8;
    if (*oldBlock == aligned_sz){
        return prev;
    }
    if (*oldBlock < aligned_sz) {
        unsigned long long *splitBlock = oldBlock + 8 + aligned_sz;
        *(unsigned long long int *)splitBlock = *oldBlock - aligned_sz - 8;

        *(unsigned long long int *)oldBlock = aligned_sz + 1 + 8;

        return prev;
    }
    if (*oldBlock > aligned_sz){
        void * newPtr = cpen212_alloc(alloc_state, aligned_sz);
        memmove(newPtr, prev, *oldBlock - 8);
    }




    // void *p = cpen212_alloc(s, nbytes);
    // if (p != NULL)
    //     memmove(p, prev, nbytes); // see WARNING below
    // return p;
}

// WARNING: we don't know the prev block's size, so memmove just copies nbytes here.
//          this is safe only because in this dumb allocator we know that prev < p,
//          and p has at least nbytes usable. in your implementation,
//          you probably need to use the original allocation size.

bool cpen212_check_consistency(void *alloc_state) {
    alloc_state_t *s = (alloc_state_t *) alloc_state;
    return s->end > s->free;
}

// int main(void){
//     void *ptr = malloc(4096);
//     struct alloc_state_s *jones = cpen212_init(ptr, ptr+4096);

//     void * c1 = cpen212_alloc(jones, 32);
//     void * c2 = cpen212_alloc(jones, 69);

//     cpen212_free(jones, c1);
//     cpen212_free(jones, c2);

    
//      printf("c2: %llu \n", *(unsigned long long int *)(c2 - 64) );
//     void * c11 = cpen212_alloc(jones, 69);
//      printf("c2: %llu \n", *(unsigned long long int *)(c2 - 64) );
//     void * c21 = cpen212_alloc(jones, 32);
//     printf("c2: %llu \n", *(unsigned long long int *)(c2 - 64) );
//     void * c3 = cpen212_alloc(jones, 20);

//     unsigned long long int size1 = *(unsigned long long int *)(c1 - 64);
//     unsigned long long int size2 = *(unsigned long long int *)(c2 - 64);
//     unsigned long long int size3 = *(unsigned long long int *)(c3 - 64);


//     printf("s1: %llu ", size1);
//     printf("s2: %llu ", size2);
//     printf("s3: %llu ", size3);
//     printf("end: %llu \n", ptr + 4096);


// }
