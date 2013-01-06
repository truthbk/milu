#include <stdlib.h>
#include <stdint.h>

#include "pool/pool.h"

struct pool * create_pool(uint16_t p_sz, size_t o_sz) {
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
