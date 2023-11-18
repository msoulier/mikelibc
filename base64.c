#include <assert.h>

#include "base64.h"

// for main
#include <string.h>
#include <stdio.h>

#define MASK 0x3F
#define ALPHASIZE 65
#define MAXCHAR 256

const unsigned char b64_alphabet[ALPHASIZE] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/', '='
    };

const unsigned int b64_alphabet_reverse[MAXCHAR] = {
    ['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,  ['E'] = 4,
    ['F'] = 5,  ['G'] = 6,  ['H'] = 7,  ['I'] = 8,  ['J'] = 9,
    ['K'] = 10, ['L'] = 11, ['M'] = 12, ['N'] = 13, ['O'] = 14,
    ['P'] = 15, ['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19,
    ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23, ['Y'] = 24,
    ['Z'] = 25, ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29,
    ['e'] = 30, ['f'] = 31, ['g'] = 32, ['h'] = 33, ['i'] = 34,
    ['j'] = 35, ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39,
    ['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43, ['s'] = 44,
    ['t'] = 45, ['u'] = 46, ['v'] = 47, ['w'] = 48, ['x'] = 49,
    ['y'] = 50, ['z'] = 51, ['0'] = 52, ['1'] = 53, ['2'] = 54,
    ['3'] = 55, ['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59,
    ['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63
    };

typedef struct orig_octets {
    unsigned char one;
    unsigned char two;
    unsigned char three;
} ORIG_OCTETS;

typedef struct b64_block {
    unsigned char one;
    unsigned char two;
    unsigned char three;
    unsigned char four;
} B64_BLOCK;


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
        output[j+2] = b64_alphabet[b64.three];
        output[j+3] = b64_alphabet[b64.four];

        if (padding == 1) {
            output[j+3] = '=';
        } else if (padding == 2) {
            output[j+2] = '=';
            output[j+3] = '=';
        }
        i += 3;
        j += 4;
    }

    output[j] = '\0';
    return output;
}
