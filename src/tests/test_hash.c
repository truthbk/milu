#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"

/* Pointer to the file used by the tests. */
static FILE* temp_file = NULL;

/* The suite initialization function.
 *  * Opens the temporary file used by the tests.
 *   * Returns zero on success, non-zero otherwise.
 *    */
int init_suite1(void)
{
}

/* The suite cleanup function.
 *  * Closes the temporary file used by the tests.
 *   * Returns zero on success, non-zero otherwise.
 *    */
int clean_suite1(void)
{
}

void testHASHCREATE(void)
{
}

void testHASHINSERT(void)
{
    unsigned char buffer[20];

    if (NULL != temp_file) {
        rewind(temp_file);
        CU_ASSERT(9 == fread(buffer, sizeof(unsigned char), 20, temp_file));
        CU_ASSERT(0 == strncmp(buffer, "Q\ni1 = 10", 9));
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

