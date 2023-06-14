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
const uint32_t GC_COUNT = 100;

/*
 * Methods for the mqueue_t.
 */
void mqueue_init(mqueue_t *queue, uint32_t initial_size, uint32_t max_size) {
    mdbgf("mallocing %d bytes of memory for the mqueue\n",
            initial_size*sizeof(void *));
    mdbgf("the size of a void * is %d bytes\n", sizeof(void *));
    queue->data = (void **)malloc(initial_size*sizeof(void *));
    assert( queue->data != NULL );
    queue->front = 0;
    queue->rear = -1;
    queue->alloc_size = initial_size;
    queue->current_size = 0;
    queue->max_size = max_size;
    queue->gc_run = GC_COUNT;
#ifdef MIKELIBC_THREADS
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
uint32_t mqueue_enqueue(mqueue_t *queue, void *item) {
    mdbgf("enqueuing item, queue->current_size is %d\n", queue->current_size);
    if (queue->max_size > 0) {
#ifdef MIKELIBC_THREADS
        pthread_mutex_lock(&(queue->mutex));
        while (queue->current_size == queue->max_size) {
            pthread_cond_wait(&(queue->full), &(queue->mutex));
        }
#else
        // Check for maximum size
        if (queue->current_size == queue->max_size) {
            return -1;
        }
#endif
    }
    // Is there enough allocated space on the array?
    if (queue->current_size == queue->alloc_size) {
        // Double the size.
        mdbgf("realloc from %d to %d\n",
                queue->alloc_size, queue->alloc_size*2);
        queue->data = realloc(queue->data, queue->alloc_size*2);
        assert( queue->data != NULL );
        queue->alloc_size *= 2;
    }
    queue->rear++;
    queue->data[queue->rear] = item;
    queue->current_size++;
#ifdef MIKELIBC_THREADS
    pthread_cond_signal(&(queue->empty));
    pthread_mutex_unlock(&(queue->mutex));
#endif
    return queue->current_size;
}

void *mqueue_dequeue(mqueue_t *queue) {
    mdbgf("dequeueing item, queue->current_size is %d\n", queue->current_size);
    void *item;
#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&(queue->mutex));
    while (queue->current_size == 0) {
        pthread_cond_wait(&(queue->empty), &(queue->mutex));
    }
#else
    if (queue->current_size == 0) {
        return NULL;
    }
#endif
    item = queue->data[queue->front];
    queue->front++;
    queue->current_size--;
    queue->gc_run--;
    // Housekeeping
    if (queue->gc_run == 0) {
        mqueue_vacuum(queue);
        queue->gc_run = GC_COUNT;
    }
#ifdef MIKELIBC_THREADS
    pthread_cond_signal(&(queue->full));
    pthread_mutex_unlock(&(queue->mutex));
#endif
    return item;
}

void mqueue_vacuum(mqueue_t *queue) {
    // Note: This runs inside of the acquired mutex if running
    // multi-threaded.
    mdbgf("in mqueue_vacuum\n");
    if (queue->front == 0) {
        // Nothing to do.
        return;
    }
    // Converting a void** to a void* for memmove.
    void *dest = queue->data;
    void *src = queue->data + queue->rear;
    size_t bytes2copy = sizeof(void *) * (queue->rear+1 - queue->front);
    mdbgf("moving %d bytes to fix slack\n");
    dest = memmove(dest, src, bytes2copy);
    queue->front = 0;
    queue->rear = bytes2copy-1;
}
