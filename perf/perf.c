#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "madt.h"
#include "mdebug.h"

#define MAX_QUEUE 12000

// For testing the ThreadSafeQueue
void* producer(void* arg) {
    mqueue_t *queue = (mqueue_t*)arg;
    for (int i = 0; i < MAX_QUEUE * 2; i++) {
        int *item = (int *)malloc(sizeof(int));
        *item = i;
        mqueue_enqueue(queue, (void *)item);
        printf("Producer enqueued: %d\n", *item);
    }
    return NULL;
}

// For testing the ThreadSafeQueue
void* consumer(void* arg) {
    mqueue_t *queue = (mqueue_t*)arg;
    for (int i = 0; i < MAX_QUEUE * 2; i++) {
        int *item = mqueue_dequeue(queue);
        printf("Consumer dequeued: %d\n", *item);
        free(item);
    }
    return NULL;
}

int
main(int argc, char *argv[]) {
    pthread_t producerThread, consumerThread;
    mqueue_t queue;

    mqueue_init(&queue, 5, MAX_QUEUE);

    pthread_create(&producerThread, NULL, producer, &queue);
    pthread_create(&consumerThread, NULL, consumer, &queue);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    mqueue_destroy(&queue);
}
