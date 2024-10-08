#ifndef _MIKELIBC_ADT_H_
#define _MIKELIBC_ADT_H_

#ifdef MIKELIBC_THREADS
#include <threads.h>
#endif

#include <stdint.h>

#include "mdebug.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The following macros are for managing a linked list, implemented
 * through C structs that have a "next" pointer. It should work with
 * any singly-linked list that has at least a "next" pointer, with the
 * end of the list indicated by next being NULL in the last node.
 */

// Add to the linked list. Add parameters are pointers to nodes.
#define mlinked_list_add(start, new_node, current) {    \
    if (start == NULL) {                                \
        start = new_node;                               \
        current = new_node;                             \
    }                                                   \
    else {                                              \
        current = start;                                \
        while (current->next != NULL) {                 \
            current = current->next;                    \
        }                                               \
        current->next = new_node;                       \
    }                                                   \
}

/**
 * Find a node in the linked list. start and current are pointers
 * to nodes, while check is a function with signature
 * int check(handle, current), which takes the handle argument, and
 * the current pointer, and returns 1 or 0 based on whether the current
 * node is the node being sought. Note that handle is something that you
 * populate. 
 */
#define mlinked_list_find(start, current, check, handle) { \
    if (start == NULL) {                                \
        current = NULL;                                 \
    }                                                   \
    else {                                              \
        current = start;                                \
        while (current != NULL) {                       \
            if (check(handle, current)) {               \
                break;                                  \
            }                                           \
            current = current->next;                    \
        }                                               \
    }                                                   \
}

/**
 * Remove a node from the linked-list. All options are pointers to
 * nodes, except check and handle, which work like in mlinked_list_find.
 * Note that freenode is populated with a pointer to the removed node.
 */
#define mlinked_list_remove(start, current, previous, freenode, check, handle) { \
    if (start != NULL) {                                \
        current = start;                                \
        while (current != NULL) {                       \
            if (check(handle, current)) {               \
                if (previous == NULL) {                 \
                    start = current->next;              \
                    previous = current;                 \
                }                                       \
                else {                                  \
                    previous->next = current->next;     \
                }                                       \
                freenode = current;                     \
            }                                           \
            else {                                      \
                previous = current;                     \
            }                                           \
            current = current->next;                    \
        }                                               \
    }                                                   \
}

/**
 * Count the nodes in the linked-list. start and current are node pointers
 * and length is an integer that will be populated with the length of
 * the list.
 */
#define mlinked_list_length(start, current, length) {   \
    length = 0;                                         \
    current = start;                                    \
    while (current != NULL) {                           \
        length++;                                       \
        current = current->next;                        \
    }                                                   \
}

/**
 * A single node in the mbtree.
 */
typedef struct mbtree_int_node {
    int value;
    struct mbtree_int_node *left;
    struct mbtree_int_node *right;
} mbtree_int_node_t;

/**
 * A constructor for a new mbtree node.
 */
mbtree_int_node_t* new_mbtree_int_node(int value);

/**
 * A destrucctor for an mbtree node.
 */
void free_mbtree_int_node(mbtree_int_node_t *node);

/**
 * A basic in-order traversal of the mbtree.
 */
void mbtree_int_inorder_traversal(mbtree_int_node_t *root);

/**
 * A thread-safe queue. Stores void* on the queue, you need to do
 * the casting. Only thread-safe if build with MIKELIBC_THREADS=1.
 */
#define QUEUE_NAME_SIZE 128
typedef struct {
    void **data;
    uint64_t front;
    uint64_t rear;
    uint64_t max_size;
    uint64_t gc_run;
    uint64_t alloc_size;
    char description[QUEUE_NAME_SIZE];
#ifdef MIKELIBC_THREADS
    mtx_t mutex;
    cnd_t full;
    cnd_t empty;
#endif
} mqueue_t;

/**
 * Initialize a new mqueue. Takes the initial size, and an optional
 * maximum size. If max_size is 0, consider the maximum size to be
 * unlimited. The minimum initial_size is 1.
 * The description string is to help identify the queue, and is copied into
 * an internal buffer no larger than 128 bytes. This can be NULL.
 */
void mqueue_init(mqueue_t *queue,
                 uint64_t initial_size,
                 uint64_t max_size,
                 char *description);

/**
 * When done with an mqueue, clean up the memory used by it.
 */
void mqueue_destroy(mqueue_t *queue);

/**
 * Enqueue a new void* something onto the queue. If we are at the
 * maximum size, and are multi-threaded, spin on the condition
 * variable until there is room. If not multi-threaded, return -1.
 * Return the new queue count on success.
 */
uint64_t mqueue_enqueue(mqueue_t *queue, void *item);

/**
 * Dequeue a void* from the end of the queue, if anything. If we are at
 * zero size, and are multi-threaded, spin on the condition variable
 * until there is something on the queue. Otherwise return NULL. The
 * item is returned on success.
 */
void *mqueue_dequeue(mqueue_t *queue);

/**
 * Return the current size of the queue, in the number of items
 * it is holding.
 */
uint64_t mqueue_size(mqueue_t *queue);

/**
 * The queue is wasteful of memory in the name of performance. As items are
 * added to it, the internal array is grown if required. But as items are
 * removed, the space is not reallocated. This results in the queue using an
 * increasing amount of memory over time. This function gets rid of the wasted
 * space on the queue. It need not be called directly, it will be called
 * internally on a regular basis, but if an immediate reclamation of memory
 * is needed, this can be called.
 */
void mqueue_vacuum(mqueue_t *queue);

#ifdef __cplusplus
}
#endif

#endif
