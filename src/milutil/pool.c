#include <stdlib.h>
#include <stdint.h>

#include "pool/pool.h"



struct pool * init_pool(uint16_t p_sz, size_t o_sz) {
    struct pool * p = NULL;
    struct pool_entry * e = NULL;
    char * mem = NULL;

    if(!(p = malloc(sizeof(struct pool)))){
        return NULL;
    }

    INIT_QUEUE(p->avail_q);

    p->_nobjs = p_sz;
    p->_obj_sz = o_sz;

    f(!(mem = malloc(p_sz * o_sz))) {
        free(p);
        return NULL;
    }
    f(!(e = malloc( p_sz * sizeof(struct pool_entry)))) {
        free(p);
        free(e);
        return NULL;
    }

    p->_start_addr = (uintptr_t)mem;
    for( i=0 ; i<p_sz ; i++) {
        e->ptr = mem;
        put_fifo(&p->avail_q, &e->q_e);
        mem += o_sz;
        e++;
    }
    p->_end_addr = (uintptr_t)mem;
    p->n_q = o_sz;

    return p;
}

int destroy_pool(struct pool * p) {
    return 0;
}

void * pool_get_ptr(struct pool * p) {
    return NULL;
}

int pool_put_ptr(struct pool * p, void * p) {
    return 0;
}
