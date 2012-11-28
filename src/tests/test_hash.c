#include <stdio.h>
#include <string.h>
#include <inttypes.h> 
#include "CUnit/Basic.h"

#include "milu.h"
#include "hashtbl/hashtbl.h"
#include "list/list.h"

/* dirty hack to get some addresses  */
#define ADDRESS_HERE() ({ void *p; __asm__("1: mov 1b, %0" : "=r" (p)); p; })


/* The suite initialization function.
 *  * Opens the temporary file used by the tests.
 *   * Returns zero on success, non-zero otherwise.
 *    */
int init_suite1(void)
{
    return 0;
}

/* The suite cleanup function.
 *  * Closes the temporary file used by the tests.
 *   * Returns zero on success, non-zero otherwise.
 *    */
int clean_suite1(void)
{
    uint32_t i = 0;
    struct memalloc * mem = NULL;
    struct hash_entry * entry = NULL;
    struct list_head  * lh = NULL;


    //clean this mess up ;)
    hash_table_for_each_safe( entry, _milu_htable, lh, i ) {
        mem = hash_entry( entry, struct memalloc, hentry );
        hash_table_del_hash_entry( _milu_htable, entry );
        free(mem);
    }

    return 0;
}

void testHASHCREATE(void)
{
    hash_table_init(_milu_htable, 100, NULL);
}

void testHASHINSERT(void)
{
    void * ptr = NULL;
    struct memalloc * mem = NULL;

    if (!(mem = (struct memalloc *)malloc(sizeof(struct memalloc)))) {
        hash_entry_init(&mem->hentry, ptr, sizeof(ptr));
        hash_table_insert_safe( _milu_htable, &mem->hentry, ptr, sizeof(ptr) );

        mem->size = 0; 
        mem->bt_size = 0;

        ptr = mem;
        mem->ptr = ptr; //points to itself.
        mem->calladdr = (uintptr_t) ADDRESS_HERE(); //this is just a mock address, cuz we don't have a caller


        //CU_ASSERT(9 == fread(buffer, sizeof(unsigned char), 20, temp_file));
        //CU_ASSERT(0 == strncmp(buffer, "Q\ni1 = 10", 9));
    }
}

void testHASHREMOVE(void)
{
}

void testHASHEXPAND(void)
{
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
    if ((NULL == CU_add_test(pSuite, "test of fprintf()", testHASHCREATE)) ||
        (NULL == CU_add_test(pSuite, "test of fread()", testHASHINSERT)) ||
        (NULL == CU_add_test(pSuite, "test of fread()", testHASHREMOVE)) ||
        (NULL == CU_add_test(pSuite, "test of fread()", testHASHEXPAND)) ||
        (NULL == CU_add_test(pSuite, "test of fread()", testHASHCOLLIDE)) ||
        (NULL == CU_add_test(pSuite, "test of fread()", testHASHCOLLIDE)))
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

