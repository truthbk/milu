#include <stdio.h>
#include <string.h>
#include <inttypes.h> 
#include "CUnit/Basic.h"

#include "milu.h"
#include "hashtbl/hashtbl.h"
#include "hash/hash.h"
#include "list/list.h"

/* dirty hack to get some addresses  */
#define ADDRESS_HERE() ({ void *p; __asm__("1: mov 1b, %0" : "=r" (p)); p; })


static void * last_ptr = NULL;


/* The suite initialization function.
 *  * Opens the temporary file used by the tests.
 *   * Returns zero on success, non-zero otherwise.
 *    */
int init_suite1(void)
{
    _milu_htable = (struct hash_table *) malloc(sizeof(struct hash_table));
    if(!_milu_htable)
    {
        return 1;
    }

    return 0;
}

/* The suite cleanup function.
 *  * Closes the temporary file used by the tests.
 *   * Returns zero on success, non-zero otherwise.
 *    */
int clean_suite1(void)
{

    if(_milu_htable)
        free(_milu_htable);

    return 0;
}

void testHASHCREATE(void)
{
    CU_ASSERT(0 == hash_table_init(
                _milu_htable, 40, hash64_cmp, milu_hash_ptr ));
}

void testHASHINSERT(void)
{
    void * ptr = NULL;
    struct memalloc * mem = NULL;

    if ( (mem = (struct memalloc *)malloc(sizeof(struct memalloc))) ) {
        hash_entry_init(&mem->hentry, ptr, sizeof(ptr));

        mem->size = 0; 
        mem->bt_size = 0;

        ptr = mem;
        mem->ptr = ptr; //points to itself.
        mem->calladdr = (uintptr_t) ADDRESS_HERE(); //this is just a mock address, cuz we don't have a caller

        hash_table_insert_safe( _milu_htable, &mem->hentry, ptr, sizeof(ptr) );
        last_ptr = ptr;
    }
}

/*
 * must be called after testHASHINSERT
 * order matters.
 * */
void testHASHGET(void)
{
    struct hash_entry * entry = NULL;
    struct memalloc * mem = NULL;

    entry = hash_table_lookup_key_safe( _milu_htable, last_ptr, sizeof(void *) );
    CU_ASSERT(0 != entry);

    mem = hash_entry( entry, struct memalloc, hentry );
    CU_ASSERT((mem->ptr == last_ptr));
}
void testHASHREMOVE(void)
{
}

void testHASHEXPAND(void)
{
    uint32_t i = 0;
    void * ptr = NULL;
    struct memalloc * mem = NULL;
    uintptr_t addr = (uintptr_t) ADDRESS_HERE();
    size_t old_hsize = _milu_htable->buckets;

    for( i = 0 ; i < old_hsize ; i++ )
    {
        if ( (mem = (struct memalloc *)malloc(sizeof(struct memalloc))) ) 
        {
            hash_entry_init(&mem->hentry, ptr, sizeof(ptr));

            mem->size = 0; 
            mem->bt_size = 0;

            ptr = mem;
            mem->ptr = ptr; //points to itself.
            mem->calladdr = addr; //this is just a mock address.

            hash_table_insert_safe( _milu_htable, &mem->hentry, ptr, sizeof(ptr) );
            last_ptr = ptr;
        }
    }
    CU_ASSERT((_milu_htable->buckets == old_hsize));
}

void testHASHCOLLIDE(void)
{
}

void testHASHDESTROY(void)
{
    uint32_t i = 0;
    struct memalloc * mem = NULL;
    struct hash_entry * entry = NULL;
    struct list_head  * lh = NULL;


    //clean this mess up ;)
    hash_table_for_each_safe( entry, _milu_htable, lh, i ) {
        mem = hash_entry( entry, struct memalloc, hentry );
        hash_table_del_hash_entry( _milu_htable, entry );
        free(mem->bt);
        free(mem);
    }

    hash_table_finit(_milu_htable);
}

/* The main() function for setting up and running the tests.
 *  * Returns a CUE_SUCCESS on successful running, another
 *   * CUnit error code on failure.
 *    */
int main()
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
    if ((NULL == CU_add_test(pSuite, "test hashtable creation", testHASHCREATE)) ||
        (NULL == CU_add_test(pSuite, "test hashtable insertion", testHASHINSERT)) ||
        (NULL == CU_add_test(pSuite, "test hashtable retrieval", testHASHGET)) ||
        (NULL == CU_add_test(pSuite, "test hashtable expansion", testHASHEXPAND)) ||
#if 0
        (NULL == CU_add_test(pSuite, "test hashtable entry removal", testHASHREMOVE)) ||
        (NULL == CU_add_test(pSuite, "test handling of collisions", testHASHCOLLIDE)) ||
#endif
        (NULL == CU_add_test(pSuite, "test hashtable destruction", testHASHDESTROY)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

