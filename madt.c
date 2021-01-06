#include <stdlib.h>

#include "madt.h"

typedef struct mbtree_int_node {
    int value;
    struct mbtree_int_node *left;
    struct mbtree_int_node *right;
} mbtree_int_node_t;

// Constructor
mbtree_int_node_t* new_mbtree_int_node(int value) {
    return NULL;
}

// Destructor
void free_mbtree_int_node(mbtree_int_node_t *node) {
    if (node != NULL) {
        free(node);
    }
}
