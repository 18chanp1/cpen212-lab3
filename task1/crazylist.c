#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include "crazylist.h"

crazycons_t *enclosing_struct(uint64_t *car) {
    return (crazycons_t *) ((void *) car - 8);
}

uint64_t *cons(uint64_t car, uint64_t *cdr) {
    crazycons_t *cons = (crazycons_t *) malloc(sizeof(crazycons_t));
    assert(cons);
    cons->car = car;
    cons->cdr = cdr;
    assert(cons);
    return (uint64_t *) &cons->car;
}

uint64_t first(uint64_t *list) {
    return *list;
    //assert(0); // FIXME
}

uint64_t *rest(uint64_t *list) {
    crazycons_t* cell = enclosing_struct(list);
    return cell->cdr;
    //assert(0); // FIXME
}

uint64_t *find(uint64_t *list, uint64_t query) {
    uint64_t *current = list;

    while (current != NULL && *current != query ){
        crazycons_t *cell = enclosing_struct(current);
        current = cell->cdr;
    }

    return current;
    
    //assert(0); // FIXME
}

uint64_t *insert_sorted(uint64_t *list, uint64_t n) {
    uint64_t *current = list;

    if(list == NULL){
        return cons(n, NULL);
    }

    if (n < *list){
        return cons(n, list);
    }

    while (current != NULL && *(enclosing_struct(current)->cdr) < n){
        current = enclosing_struct(current)->cdr;
    }

    uint64_t *nextCell = enclosing_struct(current)->cdr;


    uint64_t *newCell = cons(n, nextCell);

    enclosing_struct(current)->cdr = newCell;
    
    //assert(0); // FIXME
}

void print_list(uint64_t *list) {
    uint64_t *current = list;


    while(current != NULL){
        printf("%llu ", *current);
        current = enclosing_struct(current)->cdr;
    }

    printf("\n");
     
    return;

   

    //assert(0); // FIXME
}

//delete later

// int main(){
//     uint64_t *list = cons(100, NULL);
//     list = cons(30, list);
//     list = cons(10, list);
//     list = cons(5, list);
//     print_list(list); // output: 5 10 30 100
//     list = insert_sorted(list, 50);
//     print_list(list); // output: 5 10 30 50 100
//     *find(list, 10) = 20;
//     print_list(list); // output: 5 20 30 50 100
//     list = insert_sorted(list, 1);
//     print_list(list); // output: 1 5 20 30 50 100
//     list = insert_sorted(list, 101);
//     print_list(list); // output: 1 5 20 30 50 100 500
// }
