#include <stdlib.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
//for backtraces: http://www.gnu.org/software/libc/manual/html_node/Backtraces.html
#include <execinfo.h> 

#include "hashtbl/hashtbl.h"
#include "list/list.h"

#include "memchk.h"

/* 
 * NOTE:
 *      Watch out, this has to work in multithreaded environments 
 *      It's quite important to make necessary adjustments once the
 *      singlethread implementation is up and running.
 *
 * */


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

void * malloc(size_t size)
{
  void * ptr = NULL, *call = NULL;

  struct memalloc * mem = NULL;
  struct hash_entry * entry = NULL;


  ptr = _malloc(size);
  if(!ptr)
  {
    return NULL;
  }

  call = calladdr();

  //create a memalloc struct, init, and put in hashtable
  if(!(mem = _malloc(sizeof(struct memalloc))))
  {
    //ERROR
  }

  //initialize struct fields.
  mem->ptr = ptr; //kinda useless, only first ptr stored.... hmmmmm :S
  mem->calladdr = call;
  //The same calling code will usually allocate the same size. *But not necessarily*
  //Not for precise accounting (Don't want to use up too many resources for accounting).
  mem->size = size; 
  mem->bt_size = get_backtrace(mem->bt);
  hash_entry_init(mem->hentry, ptr, sizeof(ptr));
  hash_table_insert_safe( _milu_htable, mem->hentry, ptr, sizeof(ptr) );

#ifdef _VERBOSE
  record_malloc(ptr, size);
#endif

  stats.alloc++;
  stats.active_alloc++;
  stats.reserved += size;
  stats.active_reserved += size;

  /* whatever we got to do with the ptr */
  return ptr;
}

void * calloc(size_t nmemb, size_t size)
{
  void * ptr = NULL, *call = NULL;

  struct memalloc * mem = NULL;
  struct hash_entry * entry = NULL;


  ptr = _calloc(nmemb, size);
  if(!ptr)
  {
    return NULL;
  }
  
  call = calladdr();

  //create a memalloc struct, init, and put in hashtable
  if(!(mem = _malloc(sizeof(struct memalloc))))
  {
    //ERROR
  }

  //initialize struct fields.
  mem->ptr = ptr;
  mem->calladdr = call;
  //The same calling code will usually allocate the same size. *But not necessarily*
  //Not for precise accounting (Don't want to use up too many resources for accounting).
  mem->size = size*nmemb; 
  mem->bt_size = get_backtrace(mem->bt);
  hash_entry_init(mem->hentry, ptr, sizeof(ptr));
  hash_table_insert_safe( _milu_htable, mem->hentry, ptr, sizeof(ptr) );

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
  void * nptr = NULL, *call = NULL;

  struct memalloc * mem = NULL, * mem_old = NULL;
  struct hash_entry * entry = NULL;
  size_t old_size = 0;


  nptr = _realloc(ptr, size);
  if(!nptr)
  {
    return NULL;
  }
  call = calladdr();

  //look for the entry...
  entry = hash_table_del_key_safe( _milu_htable, ptr, sizeof(ptr) );
  if( likely(entry) )
  {
    mem_old = hash_entry(entry, struct memalloc, hentry);
  }

  //create a memalloc struct, init, and put in hashtable
  if(!(mem = (struct memalloc *) _malloc(sizeof(struct memalloc))))
  {
    //ERROR
  }

  //initialize struct fields.
  mem->ptr = nptr; 
  mem->calladdr = call;
  mem->size = size; 
  mem->bt_size = get_backtrace(mem->bt);
  hash_entry_init(mem->hentry, nptr, sizeof(ptr));
  hash_table_insert_safe( _milu_htable, mem->hentry, nptr, sizeof(nptr) );

#ifdef _VERBOSE
  if(mem_old)
  {
    record_free(mem_old->ptr, mem_old->size);
  }
  record_malloc(nptr, size);
#endif
  
  //alloc is not increased because this is a REALLOC.
  //we need to get the old_size from the hash table...
  if(mem_old)
  {
    stats.reserved -= mem_old->size;
    stats.active_reserved -= mem_old->size;

    //cleanup
    _free(mem_old->bt);
    _free(mem_old);
  }
  stats.reserved += size;
  stats.active_reserved += size;

  // we need to update ptr and size in hashtable...

  return nptr;
}

void free(void * ptr)
{
  struct memalloc * mem = NULL;
  size_t size = 0;

  //here we do things differently... to protect against double free's or
  //unallocated memory frees we first look for the ptr in the hashtable..

  //look for the entry...
  entry = hash_table_del_key_safe( _milu_htable, ptr, sizeof(ptr) );
  if( unlikely(!entry) )
  {
    mem_report();
    //clean up hashtable, milu...
    milu_cleanup();
  }
  else
  {
    mem = hash_entry( entry, struct memalloc, hentry );
  }

  record_free(ptr);
  _free(ptr); //this will fail here if we get a bad ptr. No problem.

  stats.active_alloc--;
  stats.active_reserved -= mem->size;

  //if we get here mem will NOT be NULL, but check for sake of... whatever.
  if (likely(mem)) {
    _free(mem->bt);
    _free(mem);
  }

  return;
}

void mem_report(void)
{
  int i;

  fprintf( stdout, "Total Allocations: %lu\n", stats.alloc );
  fprintf( stdout, "Unfreed Allocations: %lu\n", stats.active_alloc );
  fprintf( stdout, "Total Memory Reserved: %lu\n", stats.reserved );
  fprintf( stdout, "Total Unfreed Memory: %lu\n", stats.active_reserved );

  //Traverse hash table showing existing leaks.
  fprintf( stdout, "\n\nMemory Leaks Found: SUMMARY\n\n" );
  hash_table_for_each_safe( entry, _milu_table, &lh, i ) {
    mem = hash_entry( entry, struct memalloc, hentry );

    fprintf( stdout, "Allocation made at %p for %ld bytes\n", mem->calladdr, mem->size );
    fprintf( stdout, "Unallocation ptr to heap address: %p\n", mem->ptr );
    for( i=0 ; i<mem->bt_size ; i++)
    {
     fprintf( stdout, "[FRAME %d] %s\n", i, mem->bt[i] );
    }

    fprintf( stdout, "\n\n");
  }
}


void milu_cleanup(void)
{
  struct memalloc * mem = NULL;
  struct hash_entry * entry = NULL;
  struct list_head lh:
  uint32_t i = 0;


  //clean this mess up ;)
  hash_table_for_each_safe( entry, _milu_table, &lh, i ) {
    mem = hash_entry( entry, struct memalloc, hentry );
    hash_table_del_hash_entry( _milu_table, entry );
    _free(mem->bt);
    _free(mem);
  }
}

void __attribute__ ((destructor)) memchk_stats(void) 
{
  mem_report();
  mile_cleanup();
}
