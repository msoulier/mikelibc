#include <stdio.h>
#include <stdlib.h>

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
 * Methods for the mqueue_t.
 */
void mqueue_init(mqueue_t *queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->count = 0;
#ifdef MIKELIBC_THREADS
    pthread_mutex_init(&(queue->mutex), NULL);
    pthread_cond_init(&(queue->full), NULL);
    pthread_cond_init(&(queue->empty), NULL);
#endif
}

void mqueue_destroy(mqueue_t *queue) {
#ifdef MIKELIBC_THREADS
    pthread_mutex_destroy(&(queue->mutex));
    pthread_cond_destroy(&(queue->full));
    pthread_cond_destroy(&(queue->empty));
#endif
}

void mqueue_enqueue(mqueue_t *queue, int item) {
#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&(queue->mutex));
    while (queue->count == MAX_QUEUE_SIZE) {
        pthread_cond_wait(&(queue->full), &(queue->mutex));
    }
    // FIXME: handle non-thread case
#endif
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->data[queue->rear] = item;
    queue->count++;
#ifdef MIKELIBC_THREADS
    pthread_cond_signal(&(queue->empty));
    pthread_mutex_unlock(&(queue->mutex));
#endif
}

int mqueue_dequeue(mqueue_t *queue) {
    int item;
#ifdef MIKELIBC_THREADS
    pthread_mutex_lock(&(queue->mutex));
    while (queue->count == 0) {
        pthread_cond_wait(&(queue->empty), &(queue->mutex));
    }
    // FIXME: handle non-thread case
#endif
    item = queue->data[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->count--;
#ifdef MIKELIBC_THREADS
    pthread_cond_signal(&(queue->full));
    pthread_mutex_unlock(&(queue->mutex));
#endif
    return item;
}
