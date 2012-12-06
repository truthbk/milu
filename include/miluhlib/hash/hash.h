#ifndef _HASH_H
#define _HASH_H
/* Fast hashing routine for ints,  longs and pointers.
   (C) 2002 William Lee Irwin III, IBM */

/*
 * Knuth recommends primes in approximately golden ratio to the maximum
 * integer representable by a machine word for multiplicative hashing.
 * Chuck Lever verified the effectiveness of this technique:
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 *
 * These primes are chosen to be bit-sparse, that is operations on
 * them can use shifts and additions instead of multiplications for
 * machines where multiplications are slow.
 */

#include <asm-generic/types.h>
#include <asm-generic/bitsperlong.h>

/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GOLDEN_RATIO_PRIME_32 0x9e370001UL
/*  2^63 + 2^61 - 2^57 + 2^54 - 2^51 - 2^18 + 1 */
#define GOLDEN_RATIO_PRIME_64 0x9e37fffffffc0001UL

#if __BITS_PER_LONG == 32
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_PRIME_32
#define hash_long(val, bits) hash_32(val, bits)
#elif __BITS_PER_LONG == 64
#define hash_long(val, bits) hash_64(val, bits)
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_PRIME_64
#else
#error Wordsize not 32 or 64
#endif

static inline uint64_t hash_64(uint64_t val, unsigned int bits)
{
  uint64_t hash = val;

  /*  Sigh, gcc can't optimize this alone like it does for 32 bits. */
  uint64_t n = hash;
  n <<= 18;
  hash -= n;
  n <<= 33;
  hash -= n;
  n <<= 3;
  hash += n;
  n <<= 3;
  hash -= n;
  n <<= 4;
  hash += n;
  n <<= 2;
  hash += n;

  /* High bits are more random, so use them. */
  return hash >> (64 - bits);
}

static inline uint32_t hash_32(uint32_t val, unsigned int bits)
{
  /* On some cpus multiply is faster, on others gcc will do shifts */
  uint32_t hash = val * GOLDEN_RATIO_PRIME_32;

  /* High bits are more random, so use them. */
  return hash >> (32 - bits);
}

static inline unsigned long hash_ptr(const void *ptr, unsigned int bits)
{
  return hash_long((unsigned long)ptr, bits);
}

int hash64_cmp(const void * key_a, const void * key_b, size_t size) {
    if(size != sizeof(uint64_t)){
        //we've got a problem.
        return 0;
     }
    return (*(uint64_t *)key_a - *(uint64_t *)key_b);
}

#endif /* _HASH_H */

