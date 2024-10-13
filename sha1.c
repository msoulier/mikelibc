#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <openssl/evp.h>
#include <string.h>
#include <pthread.h>

#include "mutil.h"

unsigned char *digest = NULL;
unsigned int size = 0;
char *input = NULL;
char *hdigest = NULL;

int
main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: sha1 <space separated strings as input>\n");
        return 1;
    }
    for (int i = 1; i < argc; ++i) {
        size += strlen(argv[i]);
    }
    size++; // for the NULL
    //printf("allocating %d bytes for full string\n", size);
    input = (char *)malloc(size*sizeof(char));
    assert( input != NULL );
    for (int i = 1; i < argc; ++i) {
        if (i == 1) {
            strcpy(input, argv[i]);
        } else {
            strcat(input, argv[i]);
        }
    }
    //printf("input string is '%s'\n", input);
    digest = digest_sha1((unsigned char *)input, strlen(input), &size);
    hdigest = tohex(digest, size);
    printf("%s\n", hdigest);
    OPENSSL_free(digest);
    free(hdigest);
    free(input);
}
