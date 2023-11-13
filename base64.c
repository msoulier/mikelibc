#include "base64.h"

// for main
#include <string.h>
#include <stdio.h>

unsigned char *base64_encode(const unsigned char *data,
                             size_t input_size)
{
    ORIG_OCTETS orig;
    B64_BLOCK b64 = {0, 0, 0, 0};
    int i = 0;
    while (i < input_size) {
        // Need 3 bytes as input.
        int remaining = input_size - i;
        printf("remaining is %d bytes\n", remaining);
        if (remaining >= 3) {
            orig.one = data[i];
            orig.two = data[i+1];
            orig.three = data[i+2];
        } else if (remaining == 2) {
            orig.one = data[i];
            orig.two = data[i+1];
            orig.three = 0;
        } else {
            orig.one = data[i];
            orig.two = 0;
            orig.three = 0;
        }
        i += 3;
    }
    printf("orig size %ld, b64 size %ld\n", sizeof(ORIG_OCTETS),
                                            sizeof(B64_BLOCK));
    printf("orig: %c %c %c\n", orig.one, orig.two, orig.three);
    memcpy(&b64, &orig, sizeof(orig));
    printf("b64 raw: %d %d %d %d\n", b64.one, b64.two, b64.three, b64.four);
    printf("b64: %c %c %c %c\n", b64_alphabet[b64.one],
                                 b64_alphabet[b64.two],
                                 b64_alphabet[b64.three],
                                 b64_alphabet[b64.four]);
    return NULL;
}

// for testing
int main(int argc, char *argv[]) {
    if (argc > 1) {
        unsigned char *encoded = base64_encode((unsigned char *)argv[1], strlen(argv[1]));
        if (encoded != NULL) {
            printf("%s\n", (char *)encoded);
        } else {
            fprintf(stderr, "error in encoding\n");
        }
    }
    return 0;
}
