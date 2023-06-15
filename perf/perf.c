#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#include "madt.h"
#include "mdebug.h"

#define MAX_QUEUE 100000
#define TEST_RUNS 100

// For testing the ThreadSafeQueue
void* producer(void* arg) {
    mqueue_t *queue = (mqueue_t*)arg;
    for (int i = 0; i < MAX_QUEUE * 2; i++) {
        int *item = (int *)malloc(sizeof(int));
        *item = i;
        mqueue_enqueue(queue, (void *)item);
        //printf("Producer enqueued: %d\n", *item);
    }
    return NULL;
}

// For testing the ThreadSafeQueue
void* consumer(void* arg) {
    mqueue_t *queue = (mqueue_t*)arg;
    for (int i = 0; i < MAX_QUEUE * 2; i++) {
        int *item = mqueue_dequeue(queue);
        //printf("Consumer dequeued: %d\n", *item);
        free(item);
    }
    return NULL;
}

int
main(int argc, char *argv[]) {
    double total = 0;
    for (int i = 0; i < TEST_RUNS; i++) {
        struct timeval start;
        struct timeval end;
        bzero(&start, sizeof(struct timeval));
        bzero(&end, sizeof(struct timeval));

        if (gettimeofday(&start, NULL) < 0) {
            perror("gettimeofday");
            continue;
        }

        pthread_t producerThread, consumerThread;
        mqueue_t queue;

        mqueue_init(&queue, 5, MAX_QUEUE);

        pthread_create(&producerThread, NULL, producer, &queue);
        pthread_create(&consumerThread, NULL, consumer, &queue);

        pthread_join(producerThread, NULL);
        pthread_join(consumerThread, NULL);

        mqueue_destroy(&queue);

        if (gettimeofday(&end, NULL) < 0) {
            perror("gettimeofday");
            continue;
        }

        suseconds_t diff = end.tv_usec - start.tv_usec;
        double mdiff = diff / 1000.0;
        if (diff > 0) {
            printf("Run %d: pushed %d items through the queue\n", i, MAX_QUEUE);
            printf("    took %0.2f ms\n", mdiff);
            total += mdiff;
        } else {
            printf("Run %d: bad timing data.\n", i);
        }
    }
    printf("Ran %d runs in an average of %0.2f ms\n",
            TEST_RUNS, total / TEST_RUNS);
}
