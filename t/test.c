#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "mutil.h"
#include "madt.h"
#include "mnet.h"
#include "mdebug.h"
#include "CUnit/Basic.h"

typedef struct mll_node {
    int value;
    struct mll_node *next;
} mll_node_t;

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

/*
 * Test the mlinked-list macros.
 */
void test_mlinked_list(void) {
    mll_node_t *start = NULL;
    mll_node_t *current = NULL;
    mll_node_t *freenode = NULL;
    mll_node_t *previous = NULL;
    int length;

    mll_node_t one = { 1, NULL };
    mll_node_t two = { 2, NULL };
    mll_node_t three = { 3, NULL };

    mlinked_list_add(start, &one, current);
    mlinked_list_add(start, &two, current);
    mlinked_list_add(start, &three, current);

    mlinked_list_length(start, current, length);
    CU_ASSERT( length == 3 );

    int check(int handle, mll_node_t *current) {
        if (current->value == handle) {
            return 1;
        } else {
            return 0;
        }
    }

    mlinked_list_find(start, current, check, 2);

    CU_ASSERT( current->value == 2 );

    mlinked_list_find(start, current, check, 3);

    CU_ASSERT( current->value == 3 );
    CU_ASSERT( current->next == NULL );

    mlinked_list_remove(start, current, previous, freenode, check, 2);

    CU_ASSERT( freenode->value == 2 );

    mlinked_list_length(start, current, length);

    CU_ASSERT( length == 2 );
}

/*
 * Test the sfibonacci function.
 */
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

/*
 * Test the fibonacci function.
 */
void test_fib(void) {
    CU_ASSERT( fibonacci(0) == 0 );
    CU_ASSERT( fibonacci(1) == 1 );
    CU_ASSERT( fibonacci(2) == 1 );
    CU_ASSERT( fibonacci(3) == 2 );
    CU_ASSERT( fibonacci(4) == 3 );
}

/*
 * Test the dstnow function.
 */
void test_dstnow(void) {
    setenv("TZ", "UTC", 1);
    CU_ASSERT( dstnow() == 0 );
    // Difficult to reliably test this in other zones without
    // changing the system clock.
}

/*
 * Test the dns functions.
 */
void test_dns(void) {
    int rv = maddrlookup("google.com", "80", TCP);
    CU_ASSERT( rv == 0 );
    rv = maddrlookup("www.cbc.ca", "80", TCP);
    CU_ASSERT( rv == 0 );
    rv = maddrlookup("does.not.exist.foo", "22", TCP);
    CU_ASSERT( rv == EAI_NONAME )
    rv = maddrlookup("amazon.com", NULL, TCP);
    CU_ASSERT( rv == 0 );
}

/*
 * Test popenRWE
 */
void test_popenRWE(void) {
    int fds[3] = { 0, 0, 0 };
    const char * const args[2] = { "/bin", "/usr/bin" };
    int rv = popenRWE(fds, "/bin/ls", args);
    printf("rv was %d\n", rv);
    char buffer[1024];
    FILE *out = fdopen(fds[1], "r");
    if (out == NULL) {
        perror("fdopen");
        return;
    }
    for (;;) {
        if (fgets(buffer, 1024, out) == NULL) {
            break;
        } else {
            printf(buffer);
        }
    }
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
        (NULL == CU_add_test(pSuite, "test of fibonacci", test_fib)) ||
        (NULL == CU_add_test(pSuite, "test of dstnow", test_dstnow)) ||
        (NULL == CU_add_test(pSuite, "test of mlinked-list macros", test_mlinked_list)) ||
        (NULL == CU_add_test(pSuite, "test of dns functions", test_dns)) ||
        (NULL == CU_add_test(pSuite, "test of popenRWE", test_popenRWE))
      )
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

