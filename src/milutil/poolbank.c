#include <stdlib.h>
#include <string.h>

#include "pool/poolbank.h"
#include "pool/pool.h"

static bank_allocator _b_allocator = malloc;

struct bank * create_bank( uint16_t n_pools
                         , int8_t   growing
                         , uint16_t poolsize
                         , size_t   objsize ){

    struct bank * b = NULL;

    if(!(b = _b_allocator(sizeof(struct bank))))
    {
        return NULL;
    }
    memset(b, 0, sizeof(struct bank));

    b->_max_pools = (growing ? 0 : n_pools);
    b->_objsz = objsize;
    b->_poolsz = poolsize;

    if(n_pools > 0)
    {
        if(!(b->bank = _b_allocator(n_pools*sizeof(struct pool *))))
        {
                free(b);
                return NULL;
        }

        for(int i=0 ; i<n_pools ; i++)
        {
            b->bank[i] = create_pool(poolsize, objsize);
            if(!(b->bank[i])){
                for(int j = 0 ; j < i ; j++) {
                    free(b->bank[i]);
                    free(b);
                    return NULL;
                }
            }
            b->_allocd_pools++;
        }
    }
    return b;
}


int destroy_bank(struct bank * b) {
    int ret = 0;
    int err = 0;

    if(!b){
        return -1;
    }

    for(int i=0 ; i<b->_allocd_pools ; i++)
    {
        ret = destroy_pool(b->bank[i]);
        if(ret)
            err++;

        b->_allocd_pools--;
    }
    free(b->bank);
    free(b);

    return err;
}

int add_pool(struct bank * b) {
    struct pool ** aux_b = NULL;
    int i=0;

    if(!b || (b->_max_pools && (b->_allocd_pools == b->_max_pools))) {
        return -1;
    }

    if(!(aux_b = _b_allocator(
                    (b->_allocd_pools+1)*sizeof(struct pool *)))) {
        return -1;
    }

    for(i=0; i<b->_allocd_pools ; i++) {
        aux_b[i]=b->bank[i];
    }

    aux_b[i] = create_pool(b->_poolsz, b->_objsz);
    if(!aux_b[i]) {
        free(aux_b);
        return -1;
    }

    b->_allocd_pools++;
    free(b->bank);
    b->bank = aux_b;

    return 0;
}

/*
 * gotta decide about insertion order.
 * For the time being, we try to linear insertion. Not a
 * big deal cause pool banks should contain a small number
 * of pools. Give this some thought though.
 * */
void * bank_get_ptr(struct bank * b) {
    void * ptr = NULL;

    if(!b) {
        return NULL;
    }

    //yes O(n) put n is small.
    for(int i = 0 ; i<b->_allocd_pools ; i++) {
        ptr = pool_get_ptr(b->bank[i]);
        if(ptr)
            break;
    }

    return ptr;
}

int bank_put_ptr(struct bank * b, void * p) {
    int ret = 0;

    if(!b) {
        return -1;
    }

    //yes O(n) put n is small.
    for(int i = 0 ; i<b->_allocd_pools ; i++) {
        ret = pool_put_ptr(b->bank[i], p);
        if(!ret)
            break;
    }

    return ret;
}

void custom_b_allocator(bank_allocator allocator) {
    if(!allocator)
        return;
    _b_allocator = allocator;
    custom_p_allocator(allocator);

    return;
};
