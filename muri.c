#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mutil.h"

int
main(int argc, char *argv[]) {
    int maximum_read = 10*1024*1024;
    char *input_buffer = malloc(sizeof(char)*maximum_read);
    assert( input_buffer != NULL );
    if (argc < 2) {
        if (fgets(input_buffer, maximum_read, stdin) == NULL) {
            return 1;
        }
        // Throw away the newline.
        if (input_buffer[strnlen(input_buffer, maximum_read)-1] == '\n') {
            input_buffer[strnlen(input_buffer, maximum_read)-1] = '\0';
        } // FIXME if not then we didn't read all of stdin
    } else {
        if (strncmp(argv[1], "-h", 2) == 0) {
            fprintf(stderr, "Usage: %s [string]\nIf [string] not provided, reads from stdin.\n", argv[0]);
            return 1;
        } else {
            // string is on the command-line
            strncpy(input_buffer, argv[1], maximum_read);
        }
    }

    int return_value = 0;
    int output_length = 0;
    char *output = uriencode(input_buffer, &output_length);
    if (output != NULL) {
        printf("%s\n", output);
    } else {
        fprintf(stderr, "encoding error\n");
        return_value = 1;
    }

CLEANUP:
    if (input_buffer != NULL) {
        free(input_buffer);
    }
    if (output != NULL) {
        free(output);
    }

    return return_value;
}
