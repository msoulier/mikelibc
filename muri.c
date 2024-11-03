#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mutil.h"

int
main(int argc, char *argv[]) {
    int encode = 1;
    int read_stdin = 1;
    int maximum_read = 10*1024*1024;

    char *usage = "Usage: muri [-h] [-d]\n    reads from stdin, encode is default, -d to decode\n";

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (strncmp(argv[i], "-h", 2) == 0) {
                fprintf(stderr, usage);
                return 1;
            } else if (strncmp(argv[i], "-d", 2) == 0) {
                encode = 0;
            }
        }
    }

    char *input_buffer = malloc(sizeof(char)*maximum_read);
    assert( input_buffer != NULL );
    
    // Read from stdin.
    if (fgets(input_buffer, maximum_read, stdin) == NULL) {
        return 1;
    }
    // Throw away the newline.
    if (input_buffer[strnlen(input_buffer, maximum_read)-1] == '\n') {
        input_buffer[strnlen(input_buffer, maximum_read)-1] = '\0';
    } else {
        fprintf(stderr, "ERROR: Input too large.\n");
        // FIXME: realloc or allow configurable buffer size
        return 1;
    }

    int return_value = 0;
    int output_length = 0;
    char *output = NULL;

    if (encode) {
        output = uriencode(input_buffer, &output_length);
    } else {
        output = uridecode(input_buffer, &output_length);
    }

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
