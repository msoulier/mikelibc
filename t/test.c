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
#include "mstring.h"
#include "mlog.h"
#include "CUnit/Basic.h"

#define MAX_QUEUE 120

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

void test_tcp_client(void) {
    int sockfd = connect_tcp_client("digitaltorque.ca", "80");
    CU_ASSERT( sockfd > 0 );
    if (sockfd > 0) {
        printf("successfully connected via TCP to port 80 on digitaltorque.ca\n");
    }
}

// For testing the ThreadSafeQueue
void* producer(void* arg) {
    mqueue_t *queue = (mqueue_t*)arg;
    for (int i = 0; i < MAX_QUEUE * 2; i++) {
        int *item = (int *)malloc(sizeof(int));
        *item = i;
        int rv = mqueue_enqueue(queue, (void *)item);
        printf("Producer enqueued: %d\n", *item);
        CU_ASSERT( rv > 0 );
    }
    return NULL;
}

// For testing the ThreadSafeQueue
void* consumer(void* arg) {
    mqueue_t *queue = (mqueue_t*)arg;
    for (int i = 0; i < MAX_QUEUE * 2; i++) {
        int *item = mqueue_dequeue(queue);
        CU_ASSERT( item != NULL );
        CU_ASSERT( *item == i );
        printf("Consumer dequeued: %d\n", *item);
        free(item);
    }
    return NULL;
}

void test_mqueue(void) {
    pthread_t producerThread, consumerThread;
    mqueue_t queue;

    mqueue_init(&queue, 5, MAX_QUEUE);

    pthread_create(&producerThread, NULL, producer, &queue);
    pthread_create(&consumerThread, NULL, consumer, &queue);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    CU_ASSERT( queue.current_size == 0 );

    mqueue_destroy(&queue);
}

void test_base64_encode(void) {
    char *input = "Hello, World!";
    char *output = base64_encode(input, strlen(input));
    printf("base64 of %s is '%s'\n", input, output);
    CU_ASSERT( output != NULL );
    CU_ASSERT( strcmp(output, "SGVsbG8sIFdvcmxkIQ==") == 0 );
    free(output);
}

void test_base64_decode(void) {
    char *input = "SGVsbG8sIFdvcmxkIQ==";
    char *output = base64_decode(input, strlen(input));
    printf("base64_decode of %s is %s", input, output);
    free(output);
}

void test_msplit(void) {
    char tstring[] = "--file=- --debug --tcp";
    char **split = msplit(tstring, NULL);
    CU_ASSERT( split != NULL );
    CU_ASSERT( strncmp(split[0], "--file=-", 8) == 0 );
    printf("string 0 is '%s'\n", split[0]);
    CU_ASSERT( split[0][8] == '\0' );

    CU_ASSERT( strncmp(split[1], "--debug", 7) == 0 );
    printf("string 1 is '%s'\n", split[1]);
    CU_ASSERT( split[1][7] == '\0' );

    CU_ASSERT( strncmp(split[2], "--tcp", 5) == 0 );
    printf("string 2 is '%s'\n", split[2]);
    CU_ASSERT( split[2][5] == '\0' );

    free_msplit(split);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
    CU_pSuite pSuite = NULL;

    g_handle = get_mlogger(MLOG_STDERR, MLOG_DEBUG, LOCNOZONE);

    mdebugf("===================> starting tests <=============");

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
         (NULL == CU_add_test(pSuite, "test of popenRWE", test_popenRWE)) ||
         (NULL == CU_add_test(pSuite, "test of msplit", test_msplit)) ||
         (NULL == CU_add_test(pSuite, "test of mqueue", test_mqueue)) ||
         (NULL == CU_add_test(pSuite, "test of connect_tcp_client", test_tcp_client)) ||
         (NULL == CU_add_test(pSuite, "test of base64_encode", test_base64_encode)) ||
         (NULL == CU_add_test(pSuite, "test of base64_decode", test_base64_decode))
       )
    {
       CU_cleanup_registry();
       return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    shutdown_all_mloggers();

    return CU_get_error();
}
