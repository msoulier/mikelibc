#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "madt.h"

// Constructor
mbtree_int_node_t* new_mbtree_int_node(int value) {
    mbtree_int_node_t *new_node = (mbtree_int_node_t *)malloc(sizeof(mbtree_int_node_t));
    new_node->value = value;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Destructor
void free_mbtree_int_node(mbtree_int_node_t *node) {
    if (node != NULL) {
        free(node);
    }
}

void mbtree_int_inorder_traversal(mbtree_int_node_t *root) {
    if (root == NULL) {
        return;
    } else {
        mbtree_int_inorder_traversal(root->left);
        printf("%d\n", root->value);
        mbtree_int_inorder_traversal(root->left);
    }
}

/*
 * The number of dequeue operations on the queue that are done before
 * performing a vacuum operation to reclaim memory.
 */
const uint64_t GC_COUNT = 100;

// Internal version of mqueue_size, which assumes that the mutex is
// already acquired.
uint64_t mqueue_size_int(mqueue_t *queue) {
    return queue->rear+1 - queue->front;
}

uint64_t mqueue_size(mqueue_t *queue) {
#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&(queue->mutex));
#endif
    uint64_t size = mqueue_size_int(queue);
#ifdef MIKELIBC_THREADS
    pthread_mutex_unlock(&(queue->mutex));
#endif
    return size;
}

/*
 * Methods for the mqueue_t.
 */
void mqueue_init(mqueue_t *queue,
                 uint64_t initial_size,
                 uint64_t max_size,
                 char *description)
{
    mdbgf("%s: mqueue_init: initial_size %d and max_size %d\n",
        description, initial_size, max_size);
    if (initial_size < 1) {
        initial_size = 1;
    }
    queue->front = 0;
    queue->rear = -1;
    queue->alloc_size = initial_size;
    queue->max_size = max_size;
    queue->gc_run = GC_COUNT;
    bzero(queue->description, QUEUE_NAME_SIZE);
    if (description != NULL) {
        strncpy(queue->description, description, QUEUE_NAME_SIZE);
    }
    size_t data_size = queue->alloc_size * sizeof(void *);
    mdbgf("%s: mallocing %d bytes of memory for the mqueue\n",
        queue->description, data_size);
    queue->data = malloc(data_size);
    assert( queue->data != NULL );
#ifdef MIKELIBC_THREADS
    mdbgf("%s: initializing pthread primitives\n", queue->description);
    pthread_mutex_init(&(queue->mutex), NULL);
    pthread_cond_init(&(queue->full), NULL);
    pthread_cond_init(&(queue->empty), NULL);
#endif
}

void mqueue_destroy(mqueue_t *queue) {
    free(queue->data);
#ifdef MIKELIBC_THREADS
    pthread_mutex_destroy(&(queue->mutex));
    pthread_cond_destroy(&(queue->full));
    pthread_cond_destroy(&(queue->empty));
#endif
}

// Always enqueue to the end of the array, and grow if required.
uint64_t mqueue_enqueue(mqueue_t *queue, void *item) {
    mdbgf("%s: enqueuing item\n", queue->description);
#ifdef MIKELIBC_THREADS
    mdbgf("%s: locking mutex for enqueue\n", queue->description);
    pthread_mutex_lock(&(queue->mutex));
    mdbgf("%s: got the lock\n", queue->description);
    if (queue->max_size > 0) {
        while (mqueue_size_int(queue) == queue->max_size) {
            mdbgf("%s: queue at max size of %d - waiting\n",
                queue->description, queue->max_size);
            pthread_cond_wait(&(queue->full), &(queue->mutex));
        }
#else
    if (queue->max_size > 0) {
        // Check for maximum size
        if (mqueue_size_int(queue) == queue->max_size) {
            return -1;
        }
#endif
    }
    // Is there enough allocated space on the array?
    queue->rear++;
    if (queue->rear == queue->alloc_size) {
        // Double the size.
        size_t data_size = queue->alloc_size * sizeof(void *);
        mdbgf("%s: realloc from %d to %d\n", queue->description, data_size, data_size*2);
        queue->data = realloc(queue->data, data_size*2);
        assert( queue->data != NULL );
        queue->alloc_size *= 2;
    }
    queue->data[queue->rear] = item;
#ifdef MIKELIBC_THREADS
    mdbgf("%s: signaling empty cond and releasing mutex\n", queue->description);
    uint64_t size = mqueue_size_int(queue);
    pthread_cond_signal(&(queue->empty));
    pthread_mutex_unlock(&(queue->mutex));
#endif
    return size;
}

void *mqueue_dequeue(mqueue_t *queue) {
    mdbgf("%s: dequeueing item\n", queue->description);
    void *item;
#ifdef MIKELIBC_THREADS
    mdbgf("%s: locking mutex for dequeue\n", queue->description);
    pthread_mutex_lock(&(queue->mutex));
    mdbgf("%s: got the lock\n", queue->description);
    while (mqueue_size_int(queue) == 0) {
        mdbgf("%s: is empty, waiting\n", queue->description);
        pthread_cond_wait(&(queue->empty), &(queue->mutex));
    }
#else
    if (mqueue_size_int(queue) == 0) {
        return NULL;
    }
#endif
    item = queue->data[queue->front];
    queue->front++;
    queue->gc_run--;
    // Housekeeping
    if (queue->gc_run == 0) {
        mqueue_vacuum(queue);
        queue->gc_run = GC_COUNT;
    }
#ifdef MIKELIBC_THREADS
    mdbgf("%s: signaling full cond and releasing mutex\n", queue->description);
    pthread_cond_signal(&(queue->full));
    pthread_mutex_unlock(&(queue->mutex));
#endif
    return item;
}

void mqueue_vacuum(mqueue_t *queue) {
    // Note: This runs inside of the acquired mutex if running
    // multi-threaded.
    mdbgf("%s: in mqueue_vacuum: queue->front is %d, queue->rear is %d\n",
            queue->description, queue->front, queue->rear);
    if (queue->front == 0) {
        // Nothing to do.
        mdbgf("%s: nothing to do\n", queue->description);
        return;
    }
    // Converting a void** to a void* for memmove.
    void *dest = queue->data;
    void *src = queue->data + queue->front;
    size_t bytes2copy = sizeof(void *) * mqueue_size_int(queue);
    mdbgf("%s: moving %d bytes to fix slack\n", queue->description, bytes2copy);
    dest = memmove(dest, src, bytes2copy);
    queue->rear -= queue->front;
    queue->front = 0;
    mdbgf("%s: queue->front is now 0, queue->rear is now %d\n",
        queue->description, queue->rear);
}
