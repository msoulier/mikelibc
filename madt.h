#ifndef _MIKELIBC_ADT_H_
#define _MIKELIBC_ADT_H_

#include "mdebug.h"

#define mlinked_list_add(start, new_node, current) {    \
    mdebugf(("in mlinked_list_add macro: %s:%d\n", __FILE__, __LINE__)); \
    if (start == NULL) {                                \
        start = new_node;                               \
        mdebugf(("start is NULL <- new_node\n"));       \
    }                                                   \
    else {                                              \
        current = start;                                \
        while (current->next != NULL) {                 \
            current = current->next;                    \
        }                                               \
        mdebugf(("found the end, appending\n"));        \
        current->next = new_node;                       \
    }                                                   \
}

#define mlinked_list_find(start, current, check, handle) { \
    mdebugf(("in mlinked_list_find macro: %s:%d\n", __FILE__, __LINE__)); \
    if (start == NULL) {                                \
        current = NULL;                                 \
    }                                                   \
    else {                                              \
        current = start;                                \
        while (current != NULL) {                       \
            if (check(handle, current)) {               \
                mdebugf(("mlinked_list_find: found it\n")); \
                break;                                  \
            }                                           \
            current = current->next;                    \
        }                                               \
    }                                                   \
}

#define mlinked_list_remove(start, current, previous, freenode, check, handle) { \
    mdebugf(("in mlinked_list_remove macro: %s:%d\n", __FILE__, __LINE__)); \
    if (start != NULL) {                                \
        current = start;                                \
        while (current != NULL) {                       \
            if (check(handle, current)) {               \
                mdebugf(("mlinked_list_remove: found it\n")); \
                if (previous == NULL) {                 \
                    start = current->next;              \
                    previous = current;                 \
                    mdebugf(("mlinked_list_remove: removing at beginning\n")); \
                }                                       \
                else {                                  \
                    previous->next = current->next;     \
                    mdebugf(("mlinked_list_remove: removing past beginning\n")); \
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

#define mlinked_list_length(start, current, length) {   \
    mdebugf(("in mlinked_list_length macro: %s:%d\n", __FILE__, __LINE__)); \
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
