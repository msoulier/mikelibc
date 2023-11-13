#ifndef MIKE_BASE64_H
#define MIKE_BASE64_H
// https://datatracker.ietf.org/doc/html/rfc4648

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#define ALPHASIZE 65

const char b64_alphabet[ALPHASIZE] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/', '='
    };

typedef struct orig_octets {
    unsigned int one : 8;
    unsigned int two : 8;
    unsigned int three : 8;
} ORIG_OCTETS;

typedef struct b64_block {
    unsigned int one : 6;
    unsigned int two : 6;
    unsigned int three : 6;
    unsigned int four : 6;
} B64_BLOCK;

unsigned char *base64_encode(const unsigned char *data,
                             size_t input_size);

#endif
