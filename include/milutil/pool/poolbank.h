#ifndef _MILU_POOLBANK_H
#define _MILU_POOLBANK_H

#include <stdint.h>
#include "pool/pool.h"


struct bank {
    uint16_t _max_pools;
    uint16_t _allocd_pools;

    struct pool **bank;

}


#endif
