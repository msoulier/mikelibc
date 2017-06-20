#ifndef _MIKELIBC_ADT_H_
#define _MIKELIBC_ADT_H_

#include "mdebug.h"

#define mlinked_list_add(start, new_node, current) {    \
    mdebugf(("in mlinked_list_add macro\n"));           \
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
    mdebugf(("in mlinked_list_find macro\n"));          \
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
    mdebugf(("in mlinked_list_remove macro\n"));        \
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
#endif
