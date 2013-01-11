#include <stdio.h>
#include <string.h>
#include <inttypes.h> 
#include "CUnit/Basic.h"

#include "pool/pool.h"
#include "pool/poolbank.h"


static struct bank * _bank = NULL;
#define N_POOLS 2
#define POOLSZ 10

struct test_struct {
    int    _testint;
    char * _testptr;
};

struct test_struct * ts = NULL;
struct test_struct * tss[N_POOLS*POOLSZ];

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 * */
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
    return 0;
}

void testPOOLBANKCREATE(void)
{
    _bank = create_bank( N_POOLS, 0, 
                         POOLSZ, 
                         sizeof(struct test_struct) );
    CU_ASSERT( _bank != NULL);
}

void testPOOLBANKGET(void)
{
    ts = bank_get_ptr(_bank);
    CU_ASSERT(ts != NULL);
}

void testPOOLBANKPUT(void)
{
    CU_ASSERT( bank_put_ptr(_bank, (void *)ts) == 0);
    ts = NULL;
}
void testPOOLBANKGETALL(void)
{
    memset(tss, 0, sizeof(struct test_struct *)*N_POOLS*POOLSZ);
    for( int i=0 ; i<N_POOLS*POOLSZ ; i++ ) {
        tss[i] = bank_get_ptr(_bank);
        CU_ASSERT( tss[i] != NULL );
    }
    ts = bank_get_ptr(_bank);
    CU_ASSERT(ts == NULL);
}

void testPOOLBANKPUTALL(void)
{
    for( int i=0 ; i<N_POOLS*POOLSZ ; i++ ) {
        CU_ASSERT( bank_put_ptr(_bank, (void *)tss[i]) == 0);
    }
}

void testPOOLBANKDESTROY(void)
{
    CU_ASSERT(destroy_bank(_bank) == 0);
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
    if ((NULL == CU_add_test(pSuite, "test pool bank creation", testPOOLBANKCREATE)) ||
        (NULL == CU_add_test(pSuite, "test pool bank object retrieval", testPOOLBANKGET)) ||
        (NULL == CU_add_test(pSuite, "test pool bank object queueing", testPOOLBANKPUT)) ||
        (NULL == CU_add_test(pSuite, "test pool bank object exhaustion", testPOOLBANKGETALL)) ||
        (NULL == CU_add_test(pSuite, "test pool bank object restoration", testPOOLBANKPUTALL)) ||
        (NULL == CU_add_test(pSuite, "test pool bank destruction", testPOOLBANKDESTROY)))
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

