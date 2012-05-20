#include <stdlib.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
//for backtraces: http://www.gnu.org/software/libc/manual/html_node/Backtraces.html
#include <execinfo.h> 

#include "memchk.h"
#include "hashtbl.h"

/* 
 * NOTE:
 *      Watch out, this has to work in multithreaded environments 
 *      It's quite important to make necessary adjustments once the
 *      singlethread implementation is up and running.
 *
 * */



#ifndef likely
#define likely(x) __builtin_expect((x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect((x), 0)
#endif

/* THIS BELONGS IN HEADER FILE (memchk.h)  */

typedef void * (* malloc_fn_t)( size_t );
typedef void * (* realloc_fn_t)( void *, size_t );
typedef void * (* calloc_fn_t)( size_t, size_t );
typedef void (* free_fn_t)( void * );


struct memstats {
  uint64_t reserved;
  uint64_t active_reserved;
  uint32_t alloc;
  uint32_t active_alloc;
}

/* 
 * This what we store in the hashtable.
 * The key will be the ptr to the allocated area, which is also stored
 * in the struct in memalloc.ptr.
 * */
struct memalloc {
  void * ptr;
  void * callee;
  size_t size;
  char ** bt;
}

/* ADD ABOVE CONTENT TO HEADER FILE (memchk.h) */

struct memstats stats; //this should be thread-safe.

inline void * _malloc(size_t size)
{
  static malloc_fn_t real_malloc = (malloc_fn_t) dlsym(RTLD_NEXT, "malloc");

  return real_malloc(size);
}

inline void * _realloc(void * ptr, size_t size)
{
  static realloc_fn_t real_realloc = (realloc_fn_t) dlsym(RTLD_NEXT, "realloc");

  return real_realloc(ptr, size);
}

inline void * _calloc(size_t nmemb, size_t size)
{
  static calloc_fn_t real_calloc = (calloc_fn_t) dlsym(RTLD_NEXT, "realloc");

  return real_calloc(nmemb, size);
}

inline void _free(void * ptr)
{
  static free_fn_t real_free = (free_fn_t) dlsym(RTLD_NEXT, "free");

  real_free(ptr);
  return;
}

#ifdef _VERBOSE
/**
 *  * malloc() call recorder
 *   */
void record_malloc(size_t size, void* ptr)
{
  if (unlikely(ptr == 0)) return;

  char buf[64];
  size_t len = snprintf(buf, sizeof(buf) / sizeof(char), "+heap %p %lu\n", ptr, size);
  fwrite(buf, sizeof(char), len, stdout);
}


/**
 *  * free() call recorder
 *   */
void record_free(void* ptr)
{
  if (unlikely(ptr == 0)) return;

  char buf[64];
  size_t len = snprintf(buf, sizeof(buf) / sizeof(char), "-heap %p 0\n", ptr);
  fwrite(buf, sizeof(char), len, stdout);
}
#endif


#define callee() \
  __builtin_extract_return_address( \
      __builtin_return_address(1) )

#define _BTRACE_DEPTH 10
void * malloc(size_t size)
{
  void * ptr = NULL;
  void * ret = callee();

  void *frames[_BTRACE_DEPTH];
  size_t bt_size = 0;

  struct memalloc * mem = NULL;

  ptr = _malloc(size);
  if(ptr)
  {
    //create a memalloc struct, init, and put in hashtable

    bt_size = backtrace(frames, _BTRACE_DEPTH);
    mem->bt = backtrace_symbols(frames, bt_size);

    //initialize reamining struct fields.
  }

#ifdef _VERBOSE
  record_malloc(ptr, size);
#endif
  
  stats.alloc++;
  stats.active_alloc++;
  stats.reserved += size;
  stats.active_reserved += size*nmemb;

  /* whatever we got to do with the ptr */
  return ptr;
}

void * calloc(size_t nmemb, size_t size)
{
  void * ptr = NULL;
  void * ret = callee();

  ptr = _calloc(nmemb, size);

#ifdef _VERBOSE
  record_malloc(ptr, size*nmemb);
#endif

  stats.alloc++;
  stats.active_alloc++;
  stats.reserved += size*nmemb;
  stats.active_reserved += size*nmemb;
  /* whatever we got to do with the ptr */
  return ptr;
}

void * realloc(void * ptr, size_t size)
{
  size_t old_size = 0;

  void * ptr_ret = NULL;
  void * ret = callee();

  ptr_ret = _realloc(ptr, size);

#ifdef _VERBOSE
  record_malloc(ptr, size);
#endif
  
  //alloc is not increased because this is a REALLOC.
  //we need to get the old_size from the hash table...
  stats.reserved -= oldsize;
  stats.active_reserved -= oldsize;
  stats.reserved += size;
  stats.active_reserved += size;

  // we need to update ptr and size in hashtable...

  return ptr;
}

void free(void * ptr)
{
  size_t size = 0;

  //clean up hashtable
  _free(ptr);
  record_free(ptr);

  stats.active_alloc--;
  stats.active_reserved -= size;

  return;
}


void __attribute__ ((destructor)) memchk_stats(void) 
{
  char buf[255];
  
  fprintf(stdout, "Total Allocations: %lu\n", stats.alloc);
  fprintf(stdout, "Unfreed Allocations: %lu\n", stats.active_alloc);
  fprintf(stdout, "Total Memory Reserved: %lu\n", stats.reserved);
  fprintf(stdout, "Total Unfreed Memory: %lu\n", stats.active_reserved);
  
  //Traverse hash table showing existing leaks.
}
