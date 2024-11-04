#include <stdio.h>
#include <assert.h>

#include "madt.h"

typedef struct mll_node {
    int value;
    struct mll_node *next;
} mll_node_t;

int test_mlinked_list(void) {
    mll_node_t *start = NULL;
    mll_node_t *current = NULL;
    mll_node_t *freenode = NULL;
    mll_node_t *previous = NULL;
    int length;

    mll_node_t one = { 1, NULL };
    mll_node_t two = { 2, NULL };
    mll_node_t three = { 3, NULL };

    mlinked_list_add(start, &one, current);
    mlinked_list_add(start, &two, current);
    mlinked_list_add(start, &three, current);

    mlinked_list_length(start, current, length);
    assert( length == 3 );

    int check(int handle, mll_node_t *current) {
        if (current->value == handle) {
            return 1;
        } else {
            return 0;
        }
    }

    mlinked_list_find(start, current, check, 2);

    assert( current->value == 2 );

    mlinked_list_find(start, current, check, 3);

    assert( current->value == 3 );
    assert( current->next == NULL );

    mlinked_list_remove(start, current, previous, freenode, check, 2);

    assert( freenode->value == 2 );

    mlinked_list_length(start, current, length);

    assert( length == 2 );

    return 0;
}

int
main(void) {
    return test_mlinked_list();
}
