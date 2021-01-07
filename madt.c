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
