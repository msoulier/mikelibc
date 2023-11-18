#ifndef MIKE_BASE64_H
#define MIKE_BASE64_H
// https://datatracker.ietf.org/doc/html/rfc4648

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

unsigned char *base64_decode(const char *b64string, size_t *output_size);

char *base64_encode(const unsigned char *data,
                    size_t input_size);

#endif
