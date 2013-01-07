#include <stdlib.h>
#include <stdint.h>

#include "pool/pool.h"



struct pool * init_pool(uint16_t p_sz, size_t o_sz) {
    struct pool * p = NULL;
    struct pool_entry * e = NULL;
    struct char * mem = NULL;

    if(!(p = malloc(sizeof(struct pool)))){
        return NULL;
    }

    INIT_QUEUE(p->pool_q);
    INIT_QUEUE(p->avail_q);

    p->_nobjs = p_sz;
    p->_obj_sz = o_sz;

    f(!(mem = malloc(p_sz * o_sz))) {
        free(p);
        return NULL;
    }
    q->_pool_mem = mem;
    p->_start_addr = (uintptr_t)mem;

    if(!(e = malloc( p_sz * sizeof(struct pool_entry)))) {
        free(p->_pool_mem);
        free(p);
        return NULL;
    }
     q->_entries = e;

     for( i=0 ; i<p_sz ; i++) {
         e->ptr = mem;
         put_fifo(&p->pool_q, &e->q_e);
         mem += o_sz;
         e++;
     }
    p->_end_addr = (uintptr_t)mem;
    p->n_q = o_sz;

    return p;
}

int destroy_pool(struct pool * p) {
    if(!p)
        return -1;

    if(!p->_pool_mem)
        return -1;
    free(p->_pool_mem);

    if(!p->_entries)
        return -1;
    free(p->_entries);

    free(p);
    return 0;
}

void * pool_get_ptr(struct pool * p) {
    struct list_head * lh = NULL;
    void * ptr = NULL;

    if(!p) {
        return NULL;
    }

    lh = get_fifo(&p->pool_q);
    if(lh) {
        struct pool_entry * p_e;

        p_e = list_entry(lh, struct pool_entry, q_e);
        ptr = p_e->ptr; 
        put_fifo( &p->avail_q, lh );
        p->n_q--;
    }
    return ptr;
}

int pool_put_ptr(struct pool * p, void * p) {

    struct list_head * lh = NULL;
    struct pool_entry * p_e = NULL;
    uintptr_t addr = (uintptr_t)p;

    if(!p) {
        return -1;
    }

    //pointer doesn't belong in this pool
    if (addr < p->_start_addr || addr >= p->_end_addr) {
        return -1; //will need to come up with error codes.
    }

    lh = get_fifo(&p->pool_q);
    if(!lh) {
        //this would be weird.
        return -1;
    }

    p_e = list_entry(lh, struct pool_entry, q_e);
    p_e->ptr = p;
    put_fifo(&p->pool_qi, lh);
    p->n_q+;

    return 0;
}
