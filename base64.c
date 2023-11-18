#include <assert.h>

#include "base64.h"

// for main
#include <string.h>
#include <stdio.h>

#define MASK 0x3F

int convert_from_b64(B64_BLOCK *b64, ORIG_OCTETS *orig) {
    // all 6 bits of one, left shift 2, most significant 2 bits of 2
    orig->one = b64->one;
    orig->one <<= 2;
    orig->one |= b64->two >> 4;

    // least sig 4 bits of 2, shift left 4, most sig 4 bits of three
    orig->two = b64->two << 4;
    orig->two |= b64->three >> 2;
    
    // least sig 2 bits of 3, 6 bits of four
    orig->three = b64->three << 6;
    orig->three |= b64->four;

    return 0;
}

int convert_to_b64(ORIG_OCTETS *orig, B64_BLOCK *b64) {
    // First 6 bits of orig->one
    b64->one = (orig->one >> 2) & MASK;
    // Last 2 bits of orig->one and first 4 of orig->two
    b64->two = (orig->one << 4 | orig->two >> 4) & MASK;
    // Last 4 bits of orig->two and first 2 of orig->three
    b64->three = (orig->two << 2 | orig->three >> 6) & MASK;
    // Last 6 bits of orig->three
    b64->four = orig->three & MASK;

    return 0;
}

unsigned char *base64_decode(const char *b64string,
                             size_t *output_size)
{
    size_t input_size = strlen(b64string);
    if (input_size % 4 != 0) {
        return NULL;
    }
    int output_bytes = 3*input_size/4;
    unsigned char *output = (unsigned char *)malloc(output_bytes);
    assert( output != NULL );
    int i = 0;
    int k = 0;
    while (i < input_size) {
        // Map 4 chars
        B64_BLOCK b64 = {0, 0, 0, 0};
        ORIG_OCTETS orig = {0, 0, 0};
        uint8_t *pb64 = (uint8_t *)&b64;
        for (int j = 0; j < 4; ++j) {
            unsigned char c = b64string[i+j];
            if (c < 0) c = 0;
            pb64[j] = b64_alphabet_reverse[c];
        }
        convert_from_b64(&b64, &orig);
        output[k] = orig.one;
        output[k+1] = orig.two;
        output[k+2] = orig.three;
        i += 4;
        k += 3;
    }
    *output_size = k;

    return output;
}

char *base64_encode(const unsigned char *data,
                    size_t input_size)
{
    ORIG_OCTETS orig;
    B64_BLOCK b64;
    int output_bytes = 4*((input_size+2)/3);
    char *output = (char *)malloc(output_bytes+1);
    assert( output != NULL );
    int i = 0;
    int j = 0;
    while (i < input_size) {
        // Need 3 bytes as input.
        int remaining = input_size - i;
        int padding = 0;
        if (remaining >= 3) {
            orig.one = data[i];
            orig.two = data[i+1];
            orig.three = data[i+2];
        } else if (remaining == 2) {
            orig.one = data[i];
            orig.two = data[i+1];
            orig.three = 0;
            padding = 1;
        } else {
            orig.one = data[i];
            orig.two = 0;
            orig.three = 0;
            padding = 2;
        }
        convert_to_b64(&orig, &b64);

        output[j] = b64_alphabet[b64.one];
        output[j+1] = b64_alphabet[b64.two];

        if (padding > 0) {
            output[j+2] = '=';
        } else {
            output[j+2] = b64_alphabet[b64.three];
        }

        if (padding == 2) {
            output[j+3] = '=';
        } else {
            output[j+3] = b64_alphabet[b64.four];
        }
        i += 3;
        j += 4;
    }

    output[j] = '\0';
    return output;
}

// for testing
int main(int argc, char *argv[]) {
    if (argc > 1) {
        char *encoded = base64_encode((unsigned char *)argv[1], strlen(argv[1]));
        if (encoded != NULL) {
            printf("encoded: %s\n", encoded);
        } else {
            fprintf(stderr, "error in encoding\n");
        }

        size_t output_size = 0;
        unsigned char *data = base64_decode(encoded, &output_size);
        printf("decoded: %s\n", (char *)data);
    }
    return 0;
}
