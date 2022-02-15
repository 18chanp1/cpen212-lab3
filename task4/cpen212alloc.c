#include <stdlib.h>
#include <string.h>
#include "cpen212alloc.h"
#include <unistd.h>

/**
 * @brief Structure of the blocks:
 * first 8 byte: Size of the block, including header and footer. The LSB of this byte contains the "occupied" flag
 * <<heap space allocated to the user>>
 * last 8 byte: size of the block, including header and footer. The LSB contains the "occupied flag."
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


    //header block
    *(unsigned long long int *)s->head = heap_end - heap_start;
    *(unsigned long long int *)(s->end - 8) = heap_end - heap_start;  // store int value 10 at address 0x16
    
    return s;
}

void cpen212_deinit(void *s) {
    free(s);
}

void *cpen212_alloc(void *alloc_state, size_t nbytes) {
    alloc_state_t *s = (alloc_state_t *) alloc_state;
    size_t aligned_sz = (nbytes + 7) & ~7;

    unsigned long long int* currentBlock = s->head;
    unsigned long long int* footer = s->end - 8;

    printf("Head: % \n", currentBlock);

    while((*currentBlock & 1 || *currentBlock < aligned_sz + 16) && (currentBlock + (*currentBlock) < s->end)){
        //sleep(1);
        printf("Traverse Block: %llu \n", currentBlock);
        printf("Ta Block: %llu \n", *currentBlock);
        printf("Tail: %llu \n", s->end);
        
        if(*currentBlock & 1){
            currentBlock = currentBlock + ((*currentBlock) - 1);
            footer = currentBlock + (*currentBlock / 2) * 2;
        } else {
            currentBlock = currentBlock + (*currentBlock);
            footer = currentBlock + (*currentBlock / 2) * 2;
        }
        
        
    }

    printf ("nullchk, %llu \n", *currentBlock);
    if(currentBlock + aligned_sz + 16 > s ->end 
        || nbytes < 0 || *currentBlock & 1 || *currentBlock < aligned_sz + 16){
        
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
    unsigned long long int* currentblock = (unsigned long long int *) ptr;
    unsigned long long int* footer = (unsigned long long int *) ptr + *currentblock;
    *(unsigned long long int *) currentblock = *currentblock - 1;
    *(unsigned long long int *) footer = *footer - 1;
    printf("End of free: %llu @ %llu \n", *currentblock, currentblock);
    printf("Foot: %llu \n", *(footer + 8));
    //cpen212_coalesce(alloc_state, currentblock);

 }

void cpen212_coalesce (void *alloc_state, void *blkhdr){
    alloc_state_t *s = (alloc_state_t *) alloc_state;
    unsigned long long int * prevfooter = blkhdr - 8;
    unsigned long long int * prevHdr = prevfooter - *prevfooter + 8;
    unsigned long long int * blkftr = blkhdr + *(unsigned long long int *) blkhdr - 8;
    unsigned long long int * c1Hdr = blkhdr;

    //printf ("c1hdr: %llu \n", *prevfooter);

    
    if (!(*prevfooter & 1) && prevfooter > s->head){
        *(unsigned long long int *) prevHdr = *prevHdr + *(unsigned long long int *) blkhdr ;
        *(unsigned long long int *) blkftr = *prevHdr;

        *(unsigned long long int *) blkhdr = 0;
        *(unsigned long long int *) prevfooter = 0;
        c1Hdr = prevHdr;
    }


    
    printf("one");
    unsigned long long int * c1ftr = c1Hdr + *c1Hdr - 8;
    printf("2");
    unsigned long long int * nxtHdr = c1ftr + 8;
    unsigned long long int * nxtFtr = nxtHdr + *nxtHdr - 8;

    // if (!(*nxtHdr & 1) && nxtHdr < s->end){
    //     *(unsigned long long int *) c1Hdr = *c1Hdr + *(unsigned long long int *) nxtHdr;
        
    //     *(unsigned long long int *) nxtFtr = *c1Hdr;

    //     *(unsigned long long int *) c1ftr = 0;
    //     *(unsigned long long int *) nxtHdr = 0;
    // }
    return;
}


 

void *cpen212_realloc(void *alloc_state, void *prev, size_t nbytes) {
    //get header
    size_t aligned_sz = (nbytes + 7) & ~7;
    unsigned long long* oldBlock = prev-8;
    unsigned long long* footer = oldBlock + *oldBlock - 8;
    if (*oldBlock == aligned_sz){
        return prev;
    }
    if (*oldBlock < aligned_sz) {
        unsigned long long *splitBlockHead = oldBlock + 16 + aligned_sz;
        unsigned long long *splitBlockFoot = footer;
        *(unsigned long long int *)splitBlockHead = *oldBlock - aligned_sz - 16;
        *(unsigned long long int *)splitBlockFoot = *oldBlock - aligned_sz - 16;

        *(unsigned long long int *)oldBlock = aligned_sz + 1 + 16;
        *(unsigned long long int *)(splitBlockHead - 8) = aligned_sz + 1 + 16;

        return prev;
    }
    if (*oldBlock > aligned_sz){
        void * newPtr = cpen212_alloc(alloc_state, aligned_sz);
        memmove(newPtr, prev, *oldBlock - 16);
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

