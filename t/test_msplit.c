#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "mstr.h"

int test_msplit(void) {
    char tstring[] = "--file=- --debug --tcp";
    char **split = msplit(tstring, NULL);
    assert( split != NULL );
    assert( strncmp(split[0], "--file=-", 8) == 0 );
    printf("string 0 is '%s'\n", split[0]);
    assert( split[0][8] == '\0' );

    assert( strncmp(split[1], "--debug", 7) == 0 );
    printf("string 1 is '%s'\n", split[1]);
    assert( split[1][7] == '\0' );

    assert( strncmp(split[2], "--tcp", 5) == 0 );
    printf("string 2 is '%s'\n", split[2]);
    assert( split[2][5] == '\0' );

    free_msplit(split);
    return 0;
}

int
main(void) {
    return test_msplit();
}
