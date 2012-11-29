#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
//for backtraces: http://www.gnu.org/software/libc/manual/html_node/Backtraces.html
#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h> 
#include <inttypes.h> 
#include <pthread.h>

#include "milu.h"
#include "hashtbl/hashtbl.h"
#include "list/list.h"

#define _DEF_HSIZE 100

/* 
 * NOTE:
 *      Watch out, this has to work in multithreaded environments 
 *      It's quite important to make necessary adjustments once the
 *      singlethread implementation is up and running.
 *
 *      The first time MILU is called we need to make sure we create the
 *      hashtables, etc...
 *
 * */

struct memstats stats; //this should be thread-safe.
static uint8_t milu_enabled = 0; //make atomic 
static uint8_t milu_initd = 0; //protect with mutex

pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;

static inline void * _malloc(size_t size)
{
    static malloc_fn_t real_malloc = NULL;
    if(unlikely(!real_malloc))
    {
        real_malloc = (malloc_fn_t) dlsym(RTLD_NEXT, "malloc");
    }

    return real_malloc(size);
}

static inline void * _realloc(void * ptr, size_t size)
{
    static realloc_fn_t real_realloc = NULL;
    if(unlikely(!real_realloc))
    {
        real_realloc = (const realloc_fn_t) dlsym(RTLD_NEXT, "realloc");
    }

    return real_realloc(ptr, size);
}

static inline void * _calloc(size_t nmemb, size_t size)
{
    static calloc_fn_t real_calloc = NULL;
    if(unlikely(!real_calloc))
    {
        real_calloc = (const calloc_fn_t) dlsym(RTLD_NEXT, "realloc");
    }

    return real_calloc(nmemb, size);
}

static inline void _free(void * ptr)
{
    static free_fn_t real_free = NULL;
    if(unlikely(!real_free))
    {
        real_free = (const free_fn_t) dlsym(RTLD_NEXT, "free");
    }

    real_free(ptr);
    return;
}

static inline int _init_htable(void)
{
    if(!_milu_htable)
    {
        _milu_htable = (struct hash_table *)_malloc(sizeof(struct hash_table));
        if(!_milu_htable)
        {
            return -1;
        }
    }

    hash_table_init(_milu_htable, _DEF_HSIZE, NULL);
    return 0;
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
void record_free(size_t size, void* ptr)
{
    if (unlikely(ptr == 0)) return;

    char buf[64];
    size_t len = snprintf(buf, sizeof(buf) / sizeof(char), "-heap %p 0\n", ptr);
    fwrite(buf, sizeof(char), len, stdout);
}
#endif

void * malloc(size_t size)
{
    void * ptr = NULL;
    uintptr_t call = 0;

    struct memalloc * mem = NULL;

    if(unlikely(!milu_initd))
    {
        pthread_mutex_lock( &init_mutex );
        //init the hashtable
        if(!milu_initd)
        {
            milu_initd = 1;
            milu_enabled = 0;
            if(_init_htable())
            {
                milu_enabled = 0;
                milu_initd = 0;
            }
            milu_enabled = 1;
        }
        pthread_mutex_unlock( &init_mutex );
    }

    ptr = _malloc(size);
    if(!ptr)
    {
        return NULL;
    }

    if(likely(milu_enabled))
    {
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
        hash_entry_init(&mem->hentry, ptr, sizeof(ptr));
        hash_table_insert_safe( _milu_htable, &mem->hentry, ptr, sizeof(ptr) );

#ifdef _VERBOSE
        record_malloc(ptr, size);
#endif

        stats.alloc++;
        stats.active_alloc++;
        stats.reserved += size;
        stats.active_reserved += size;
    }

    /* whatever we got to do with the ptr */
    return ptr;
}

void * calloc(size_t nmemb, size_t size)
{
    void * ptr = NULL;
    uintptr_t call = 0;

    struct memalloc * mem = NULL;

    if(unlikely(!milu_initd))
    {
        pthread_mutex_lock( &init_mutex );
        //init the hashtable
        if(!milu_initd)
        {
            milu_initd = 1;
            milu_enabled = 0;
            if(_init_htable())
            {
                milu_enabled = 0;
                milu_initd = 0;
            }
            milu_enabled = 1;
        }
        pthread_mutex_unlock( &init_mutex );
    }


    ptr = _calloc(nmemb, size);
    if(!ptr)
    {
        return NULL;
    }

    if(likely(milu_enabled))
    {
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
        hash_entry_init(&mem->hentry, ptr, sizeof(ptr));
        hash_table_insert_safe( _milu_htable, &mem->hentry, ptr, sizeof(ptr) );

#ifdef _VERBOSE
        record_malloc(ptr, size*nmemb);
#endif

        stats.alloc++;
        stats.active_alloc++;
        stats.reserved += size*nmemb;
        stats.active_reserved += size*nmemb;
    }
    /* whatever we got to do with the ptr */
    return ptr;
}

void * realloc(void * ptr, size_t size)
{
    void * nptr = NULL;
    uintptr_t call = 0;

    struct memalloc * mem = NULL, * mem_old = NULL;
    struct hash_entry * entry = NULL;

    if(unlikely(!milu_initd))
    {
        pthread_mutex_lock( &init_mutex );
        //init the hashtable
        if(!milu_initd)
        {
            milu_initd = 1;
            milu_enabled = 0;
            if(_init_htable())
            {
                milu_enabled = 0;
                milu_initd = 0;
            }
            milu_enabled = 1;
        }
        pthread_mutex_unlock( &init_mutex );
    }

    nptr = _realloc(ptr, size);
    if(!nptr)
    {
        return NULL;
    }


    if(likely(milu_enabled))
    {
        call = calladdr();

        //look for the entry...
        entry = hash_table_del_key_safe( _milu_htable, ptr, sizeof(ptr) );
        if( likely(!!entry) )
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
        hash_entry_init(&mem->hentry, nptr, sizeof(ptr));
        hash_table_insert_safe( _milu_htable, &mem->hentry, nptr, sizeof(nptr) );

#ifdef _VERBOSE
        if(mem_old)
        {
            record_free( mem_old->size, mem_old->ptr);
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
    }
    return nptr;
}

void free(void * ptr)
{
    struct hash_entry * entry = NULL;
    struct memalloc * mem = NULL;

    if(unlikely(!milu_initd))
    {
        pthread_mutex_lock( &init_mutex );
        //init the hashtable
        if(!milu_initd)
        {
            milu_initd = 1;
            milu_enabled = 0;
            if(_init_htable())
            {
                milu_enabled = 0;
                milu_initd = 0;
            }
            milu_enabled = 1;
        }
        pthread_mutex_unlock( &init_mutex );
    }

    if(likely(milu_enabled))
    {
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
            stats.active_alloc--;
            stats.active_reserved -= mem->size;

#ifdef _VERBOSE
            record_free(mem->size, ptr);
#endif
        }


        if (likely(!!mem)) {
            _free(mem->bt);
            _free(mem);
        }
    }

    _free(ptr); //this will fail here if we get a bad ptr. "No problem".
    return;
}

void mem_report(void)
{
    uint32_t i = 0;
    struct memalloc * mem = NULL;
    struct hash_entry * entry = NULL;
    struct list_head * lh = NULL;

    fprintf( stdout, "Total Allocations:%" PRIu64 "\n", stats.alloc );
    fprintf( stdout, "Unfreed Allocations:%" PRIu64 "\n", stats.active_alloc );
    fprintf( stdout, "Total Memory Reserved: %" PRIu64 "\n", stats.reserved );
    fprintf( stdout, "Total Unfreed Memory: %" PRIu64 "\n", stats.active_reserved );

    //Traverse hash table showing existing leaks.
    fprintf( stdout, "\n\nMemory Leaks Found: SUMMARY\n\n" );
    hash_table_for_each_safe( entry, _milu_htable, lh, i ) {
        mem = hash_entry( entry, struct memalloc, hentry );

        fprintf( stdout, "Allocation made at %" PRIuPTR " for %ld bytes\n", mem->calladdr, mem->size );
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
    uint32_t i = 0;
    struct memalloc * mem = NULL;
    struct hash_entry * entry = NULL;
    struct list_head  * lh = NULL;


    //clean this mess up ;)
    hash_table_for_each_safe( entry, _milu_htable, lh, i ) {
        mem = hash_entry( entry, struct memalloc, hentry );
        hash_table_del_hash_entry( _milu_htable, entry );
        _free(mem->bt);
        _free(mem);
    }
}

void __attribute__ ((destructor)) memchk_stats(void) 
{
    mem_report();
    milu_cleanup();
}
