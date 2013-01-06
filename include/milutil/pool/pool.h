#ifndef _MILU_POOL_H
#define _MILU_POOL_H

#include <stdint.h>
#include "list/list.h"

struct pool_entry {
    void * ptr;
    struct list_head q_e; //pool queue
}

struct pool {
    uint16_t _nobjs;
    uint16_t _n_q;
    size_t _obj_sz;

    list_head used_q; 
    list_head avail_q; 
}

struct pool * create_pool(uint16_t p_sz, size_t o_sz);

int destroy_pool(struct pool * p);

void * pool_get_ptr(struct pool * p);

int pool_put_ptr(struct pool * p, void * p);

#endif
