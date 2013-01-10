#include <stdio.h>
#include <string.h>
#include <inttypes.h> 
#include "CUnit/Basic.h"

#include "pool/pool.h"
#include "pool/poolbank.h"



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
    CU_ASSERT(1);
}

void testPOOLBANKGET(void)
{
    CU_ASSERT(1);
}

void testPOOLBANKPUT(void)
{
    CU_ASSERT(1);

}
void testPOOLBANKGETALL(void)
{
    CU_ASSERT(1);
}

void testPOOLBANKPUTALL(void)
{
    CU_ASSERT(1);
}

void testPOOLBANKDESTROY(void)
{
    CU_ASSERT(1);
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

