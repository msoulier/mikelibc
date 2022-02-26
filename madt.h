#ifndef _MIKELIBC_ADT_H_
#define _MIKELIBC_ADT_H_

#include "mdebug.h"

/*
 * The following macros are for managing a linked list, implemented
 * through C structs that have a "next" pointer. It should work with
 * any singly-linked list that has at least a "next" pointer, with the
 * end of the list indicated by next being NULL in the last node.
 */

// Add to the linked list. Add parameters are pointers to nodes.
#define mlinked_list_add(start, new_node, current) {    \
    if (start == NULL) {                                \
        start = new_node;                               \
    }                                                   \
    else {                                              \
        current = start;                                \
        while (current->next != NULL) {                 \
            current = current->next;                    \
        }                                               \
        current->next = new_node;                       \
    }                                                   \
}

/*
 * Find a node in the linked list. start and current are pointers
 * to nodes, while check is a function with signature
 * int check(handle, current), which takes the handle argument, and
 * the current pointer, and returns 1 or 0 based on whether the current
 * node is the node being sought.
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

/*
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

/*
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

typedef struct mbtree_int_node {
    int value;
    struct mbtree_int_node *left;
    struct mbtree_int_node *right;
} mbtree_int_node_t;

mbtree_int_node_t* new_mbtree_int_node(int value);

void free_mbtree_int_node(mbtree_int_node_t *node);

void mbtree_int_inorder_traversal(mbtree_int_node_t *root);

#endif
