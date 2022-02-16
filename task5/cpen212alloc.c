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
    unsigned long long int* footer;

    printf("Head: %llu \n", currentBlock);

    while((*currentBlock & 1 || *currentBlock < aligned_sz + 16) && ((void *)currentBlock + (*currentBlock) < s->end)){
        //sleep(1);
        printf("Traverse Block: %llu \n", currentBlock);
        printf("Ta Block: %llu \n", *currentBlock);
        printf("Tail: %llu \n", s->end);
        
        if(*currentBlock & 1){
            currentBlock = (void *)currentBlock + ((*currentBlock) - 1);
        } else {
            currentBlock = (void *) currentBlock + (*currentBlock);
        }
        
        
    }

    printf ("nullchk, %llu \n", *currentBlock);
    if((void *)currentBlock + aligned_sz + 16 > s ->end 
        || nbytes < 0 || *currentBlock & 1 || *currentBlock < aligned_sz + 16){
        
        return NULL;
    }

    unsigned long long int size = *currentBlock;
    footer = (void *)currentBlock + *currentBlock - 8;
    
    //size *=8;

    printf ("Assigned Block: %llu \n", currentBlock);

    if (*currentBlock == aligned_sz + 16){
        printf("Dirass \n");
        (*currentBlock)++;
        printf("Footer data Dir.Ass: %llu \n", *footer);
        return ((void *)currentBlock + 8);
    }
    if (*currentBlock > aligned_sz + 16) {
        printf("F1 \n");
        unsigned long long int *splitBlock = (void *)currentBlock + 16 + aligned_sz;
        printf("F2 \n");
        *(unsigned long long int *)splitBlock = *currentBlock - aligned_sz - 16;
        printf("F3 \n"); 
        unsigned long long int* newBlkftr = (void *)currentBlock + aligned_sz + 8;

        printf("Splitblk: %llu \n", *splitBlock);
        *(unsigned long long int *)currentBlock = aligned_sz + 1 + 16;
        *(unsigned long long int *)newBlkftr = aligned_sz + 1 + 16;
        printf("F5 . %llu \n", *footer);
        *(unsigned long long int *)footer = *splitBlock;
        printf("F6 \n");

        unsigned long long int* testificate = (void *) currentBlock + *currentBlock - 9;
        printf("Footer (testif) data: %llu \n", *testificate);

        return (void *)currentBlock + 8; //incremented
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
    void *ptr = p - 8;
    unsigned long long int* currentblock = (unsigned long long int *) ptr;
    unsigned long long int* footer = (void *)currentblock + *currentblock - 9;
    printf("Foot Data (free): %llu \n", *(footer));
    *(unsigned long long int *) currentblock = *currentblock - 1;
    *(unsigned long long int *) footer = *footer - 1;
    printf("End of free: %llu @ %llu \n", *currentblock, currentblock);
    //printf("Foot Data: %llu \n", *(footer));
    cpen212_coalesce(alloc_state, currentblock);

 }

void cpen212_coalesce (void *alloc_state, void *blkhdr){
    alloc_state_t *s = (alloc_state_t *) alloc_state;
    unsigned long long int * prevfooter = blkhdr - 8;
    unsigned long long int * prevHdr = (void *)prevfooter - *prevfooter + 8;
    unsigned long long int * blkftr = blkhdr + *(unsigned long long int *) blkhdr - 8;
    unsigned long long int * c1Hdr = blkhdr;

    //printf ("c1hdr: %llu \n", *prevfooter);
     printf("blkhdr: %llu \n", * (unsigned long long int*) blkhdr);

    
    if (!(*prevfooter & 1) && prevfooter > s->head){
        printf("C BEFORE \n");
        *(unsigned long long int *) prevHdr = *prevHdr + *(unsigned long long int *) blkhdr ;
        *(unsigned long long int *) blkftr = *prevHdr;

        *(unsigned long long int *) blkhdr = 0;
        *(unsigned long long int *) prevfooter = 0;
        c1Hdr = prevHdr;
        printf("c1hdr updated\n");
    }


    
    unsigned long long int * c1ftr = (void *)c1Hdr + *c1Hdr/2 * 2 - 8;
    unsigned long long int * nxtHdr = (void *)c1ftr + 8;
    unsigned long long int * nxtFtr = (void *)nxtHdr + *nxtHdr/2 * 2 - 8;

    printf("Next Header: %llu \n", *nxtHdr);
   

    if (!(*nxtHdr & 1) && nxtHdr < s->end){
        printf("C AFTER \n");
        *(unsigned long long int *) c1Hdr = *c1Hdr + *(unsigned long long int *) nxtHdr;
        
        *(unsigned long long int *) nxtFtr = *c1Hdr;

        *(unsigned long long int *) c1ftr = 0;
        *(unsigned long long int *) nxtHdr = 0;
    }
    return;
}


 

void *cpen212_realloc(void *alloc_state, void *prev, size_t nbytes) {
    //get header
    size_t aligned_sz = (nbytes + 7) & ~7;
    unsigned long long* oldBlock = prev-8;
    unsigned long long* footer = (void *)oldBlock + *oldBlock - 9;
    if (*oldBlock == aligned_sz + 16){
        return prev;
    }
    if (*oldBlock > aligned_sz + 16) {
        printf("ftr: %llu \n", *oldBlock);
        unsigned long long int *splitBlockHead = (void *) oldBlock + 16 + aligned_sz;
        unsigned long long int *newBlockFoot = (void *) splitBlockHead - 8;

        *(unsigned long long int *)splitBlockHead = *oldBlock - 1 - 16 - aligned_sz;
        *(unsigned long long int *)footer = *oldBlock - 1 - 16 - aligned_sz;

        *(unsigned long long int *)newBlockFoot = aligned_sz + 16 + 1;
        *(unsigned long long int *)oldBlock = aligned_sz + 16 + 1;

        return prev;
    }
    if (*oldBlock < aligned_sz + 16){
        void * newPtr = cpen212_alloc(alloc_state, aligned_sz);
        if(newPtr == NULL){
            return NULL;
        }
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

