#include <stdio.h>
#include <string.h>
#include "mutil.h"
#include "CUnit/Basic.h"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
    return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
    return 0;
}

void test_sfib(void) {
    uint64_t first = sfibonacci(1);
    uint64_t second = sfibonacci(0);
    uint64_t third = sfibonacci(0);
    uint64_t fourth = sfibonacci(0);
    uint64_t fifth = sfibonacci(0);
    CU_ASSERT( first  == 0 );
    CU_ASSERT( second == 1 );
    CU_ASSERT( third  == 1 );
    CU_ASSERT( fourth == 2 );
    CU_ASSERT( fifth  == 3 );
}

void test_fib(void) {
    CU_ASSERT( fibonacci(0) == 0 );
    CU_ASSERT( fibonacci(1) == 1 );
    CU_ASSERT( fibonacci(2) == 1 );
    CU_ASSERT( fibonacci(3) == 2 );
    CU_ASSERT( fibonacci(4) == 3 );
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
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
   if ( (NULL == CU_add_test(pSuite, "test of sfibonacci", test_sfib)) ||
        (NULL == CU_add_test(pSuite, "test of fibonacci", test_fib)) )
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

