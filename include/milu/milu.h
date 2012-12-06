#ifndef _MILU_H
#define _MILU_H

#include "hashtbl/hashtbl.h"
#include "hash/hash.h"

#ifndef likely
#define likely(x) __builtin_expect((x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect((x), 0)
#endif

//must be init'd
struct hash_table * _milu_htable = NULL;

typedef void * (* malloc_fn_t)( size_t );
typedef void * (* realloc_fn_t)( void *, size_t );
typedef void * (* calloc_fn_t)( size_t, size_t );
typedef void (* free_fn_t)( void * );


struct memstats {
  uint64_t reserved;
  uint64_t active_reserved;
  uint64_t alloc;
  uint64_t active_alloc;
};

/* 
 * This what we store in the hashtable.
 * The key will be the ptr to the allocated area, which is also stored
 * in the struct in memalloc.ptr.
 * 
 * Performance and a not very large memory footpring are quite important
 * this is why it becomes a little complicated to keep track of all heap
 * memory pointers without hurting either. Performance improvement via
 * struct memalloc pools.
 *
 * */
struct memalloc {
  void          *ptr; //kinda useless, only one ptr stored.... hmmmmm (list) :S
  uintptr_t     calladdr;

  uint8_t       bt_size;
  char          **bt;
  size_t        size;

  struct hash_entry     hentry;
#ifdef _POOLED_ALLOC
  struct list_head      plist; /* Available/In-Use Pool lists */
#endif
};

#ifdef _POOLED_ALLOC
struct pool {
  uint32_t chunk_sz;
  uint32_t total;
  struct list_head _available;
  struct list_head _used;
};
#endif

/* I think this actually returns uintptr_t, not void *  */
#ifdef __buitin_extract_return_address
#define calladdr() \
  (__builtin_extract_return_address( \
      (__builtin_return_address(0)))) 
#else
#define calladdr() \
      ((uintptr_t)__builtin_return_address(0)) 
#endif


#define _BTRACE_DEPTH 10
#define get_backtrace(bt) ({ \
    int depth; \
    void * frames[_BTRACE_DEPTH]; \
    depth =  backtrace(frames, _BTRACE_DEPTH); \
    bt = backtrace_symbols(frames, depth); \
    depth; })

#define KEY_HASHBITS 8
/*
 * @ptr : void ptr we wish to hash
 * @len : this parameter is ignored, we need to comply with prototype.
 * */
static unsigned long milu_hash_ptr(const void *ptr, size_t len)
{
    return hash_ptr(ptr, KEY_HASHBITS);
}

#ifdef _VERBOSE
void record_malloc(size_t size, void* ptr);
void record_free(size_t size, void* ptr);
#endif

void mem_report(void);
void milu_cleanup(void);

#endif
