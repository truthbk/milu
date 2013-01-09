#ifndef _MILU_POOLBANK_H
#define _MILU_POOLBANK_H

#include <stdint.h>
#include "pool/pool.h"


struct bank {
    uint16_t _max_pools;
    uint16_t _allocd_pools;
    uint16_t _poolsz;
    size_t   _objsz;


    struct pool **bank;
};

typedef void * (* bank_allocator)(size_t size);

struct bank * create_bank( uint16_t n_pools
                         , int8_t   growing
                         , uint16_t poolsize
                         , size_t   objsize );

int destroy_bank(struct bank * b);

int add_pool(struct bank * b);

void * bank_get_ptr(struct bank * b);

int bank_put_ptr(struct bank * b, void * p);

void custom_b_allocator(bank_allocator allocator);

#endif
