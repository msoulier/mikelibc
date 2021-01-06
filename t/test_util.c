#include <stdio.h>

#include "mutil.h"

int
main(void) {
    printf("fibonacci: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", fibonacci(i));
    }
    printf("\n");
}
