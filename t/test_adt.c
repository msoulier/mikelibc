#include <stdio.h>

#include "madt.h"

mbtree_int_node_t *root = NULL;

int
main(void) {
    mbtree_int_node_t *node = new_mbtree_int_node(1);
    root = node;
    root->left = new_mbtree_int_node(2);
    root->right = new_mbtree_int_node(3);
    root->left->left = new_mbtree_int_node(4);
    root->left->right = new_mbtree_int_node(5);

    mbtree_int_inorder_traversal(root);
}
