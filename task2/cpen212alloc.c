#include <stdlib.h>
#include <string.h>
#include "cpen212alloc.h"

/**
 * @brief Structure of the blocks:
 * first 8 byte: Size of the block, including header and footer. The LSB of this byte contains the "occupied" flag
 * <<heap space allocated to the user>>
 * 
 */

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


    while((*currentBlock & 1 || *currentBlock < aligned_sz + 8) && (currentBlock + (*currentBlock) / 8 < s->end)){

        
        if(*currentBlock & 1){
            currentBlock = currentBlock + ((*currentBlock) - 1)/8;
        } else {
            currentBlock = currentBlock + (*currentBlock)/8;
        }
        
        
    }

    unsigned long long int t = aligned_sz + 8;
    printf ("nullchk, %llu \n", *currentBlock);
    if(currentBlock + t/8 + 1> s ->end 
        || nbytes < 0 || *currentBlock & 1 || *currentBlock < aligned_sz + 8){
        
        return NULL;
    }

    unsigned long long int size = *currentBlock;


    if (*currentBlock == aligned_sz + 8){
        
        (*currentBlock)++;
        return currentBlock + 1;
    }
    if (*currentBlock > aligned_sz + 8) {
        unsigned long long temp = 1 + aligned_sz + 8;
        unsigned long long* splitBlock = currentBlock + temp / 8;
        printf ("Splitblokc %p", splitBlock);
        *(unsigned long long int *)splitBlock = *currentBlock - aligned_sz - 8;
        printf ("Splitblokc Val %llu \n", *splitBlock);
        
        *(unsigned long long int *)currentBlock = aligned_sz + 1 + 8;

        return currentBlock + 1;
    }

}

void cpen212_free(void *alloc_state, void *p) {
    void *ptr = p - 8;
    unsigned long long int *currentblock = (unsigned long long int *) ptr;
    *(unsigned long long int *) currentblock = *currentblock - 1;
 }

 

void *cpen212_realloc(void *alloc_state, void *prev, size_t nbytes) {
    //get header
    size_t aligned_sz = (nbytes + 7) & ~7;
    unsigned long long* oldBlock = (unsigned long long int *)prev - 1;
    if (*oldBlock == aligned_sz + 8){
        return prev;
    }
    if (*oldBlock > aligned_sz + 8) {
        unsigned long long temp = 1 + aligned_sz + 8;
        unsigned long long* splitBlock = oldBlock + (temp / 8) ;
        *(unsigned long long int *)splitBlock = *oldBlock - aligned_sz - 8;
        
        *(unsigned long long int *)oldBlock = aligned_sz + 8 + 1;

        return prev;
    }
    if (*oldBlock < aligned_sz + 8){
        void * newPtr = cpen212_alloc(alloc_state, aligned_sz);
        if (newPtr == NULL){
            return NULL;
        }
        memmove(newPtr, prev, *oldBlock - 8);

        (*oldBlock)--;

        return newPtr;
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

