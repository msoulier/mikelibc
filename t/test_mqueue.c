#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "madt.h"

#define MAX_QUEUE 10

// For testing the ThreadSafeQueue
void* producer(void* arg) {
    mqueue_t *queue = (mqueue_t*)arg;
    for (int i = 0; i < MAX_QUEUE; i++) {
        int *item = (int *)malloc(sizeof(int));
        *item = i;
        int rv = mqueue_enqueue(queue, (void *)item);
        printf("Producer enqueued: %d\n", *item);
        assert( rv > 0 );
    }
    return NULL;
}

// For testing the ThreadSafeQueue
void* consumer(void* arg) {
    mqueue_t *queue = (mqueue_t*)arg;
    for (int i = 0; i < MAX_QUEUE; i++) {
        int *item = mqueue_dequeue(queue);
        assert( item != NULL );
        assert( *item == i );
        printf("Consumer dequeued: %d\n", *item);
        free(item);
    }
    return NULL;
}

int test_mqueue(void) {
    pthread_t producerThread, consumerThread;
    mqueue_t queue;

    mqueue_init(&queue, 5, MAX_QUEUE, "test queue");

    pthread_create(&producerThread, NULL, producer, &queue);
    pthread_create(&consumerThread, NULL, consumer, &queue);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    printf("queue.current_size is %ld\n", mqueue_size(&queue));
    assert( mqueue_size(&queue) == 0 );

    mqueue_destroy(&queue);

    return 0;
}

int
main(void) {
    return test_mqueue();
}
