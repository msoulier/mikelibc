#include <stdio.h>
#include <assert.h>

#include "mutil.h"

int test_b64_enc_dec(void) {
    char *inputs[] = {
        "foo", "one two three", "four five", "six seven\\ eight",
        "this is an input string"
        };
    for (int i = 0; i < 5; ++i) {
        size_t orig_size = 0;
        size_t output_size = 0;
        printf("b64 encoding %s\n", inputs[i]);
        orig_size = strlen(inputs[i]);
        char *encoded = mbase64_encode((unsigned char *)inputs[i], strlen(inputs[i]));
        unsigned char *decoded = mbase64_decode(encoded, &output_size);
        if (decoded == NULL) {
            fprintf(stderr, "FAIL: mbase64_decode returned NULL\n");
        } else {
            decoded[output_size] = '\0';
            fprintf(stderr, "b64: orig_size %ld / result_size %ld\n",
                orig_size, output_size);
            assert( strcmp((char *)decoded, inputs[i]) == 0 );
            assert( orig_size == output_size );
            free(decoded);
        }
        free(encoded);
    }
}

int
main(void) {
    return test_b64_enc_dec();
}