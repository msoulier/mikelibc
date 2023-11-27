#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mbase64.h"
#include "mdebug.h"

#define BUFSIZE 1024


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: mb64 [-d] <input|->\n");
        fprintf(stderr, "    [-d] decode (encode is default)\n");
        exit(1);
    }

    int encode = 1;
    int test = 0;
    char *input = NULL;
    FILE *input_file = NULL;
    unsigned char buffer[BUFSIZE];

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-d") == 0) {
            encode = 0;
        } else if (strcmp(argv[i], "-t") == 0) {
            test = 1;
        } else {
            input = argv[i];
        }
    }

    // If an input string is not provided, or it is -, then read
    // input from stdin. Otherwise, read from the file path referenced.
    if ((input == NULL) || ((input[0] == '-') && (input[1] == '\0'))) {
        input_file = stdin;
    } else {
        input_file = fopen(input, "r");
        if (input_file == NULL) {
            perror("input file");
            exit(1);
        }
    }

    int fd = fileno(input_file);

    int done = 0;
    mdbgf("encode is %d\n", encode);
    mdbgf("test is %d\n", test);
    ssize_t input_size = 0;
    while (! done) {
        ssize_t bytes = read(fd, buffer, BUFSIZE);
        mdbgf("read %d bytes from input file\n", bytes);
        input_size += bytes;
        switch (bytes) {
            case 0:
                done = 1;
                break;
            case -1:
                done = 1;
                perror("read error");
                break;
            default:
                break;
        }
    }
    if (encode) {
        mdbgf("encoding '%s'\n", buffer);
        char *b64string = mbase64_encode(buffer, input_size);
        printf("%s\n", b64string);
        // If test is set, decode again for comparison.
        if (test) {
            mdbgf("test set - decoding '%s'\n", b64string);
            size_t outlen = 0;
            printf("%s\n", mbase64_decode(b64string, &outlen));
            printf("input size: %ld\n", input_size);
            printf("output size: %ld\n", outlen);
            if (input_size != outlen) {
                fprintf(stderr, "ERROR: sizes do not match\n");
            }
        }
    }
    fflush(stdout);

    return 0;
}
