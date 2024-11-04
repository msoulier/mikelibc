#include <stdio.h>
#include <assert.h>

#include "mutil.h"

int test_sha1_hexdigest(void) {
    unsigned char input[] = "This is my input";
    unsigned char *output = NULL;
    unsigned int outsize = 0;
    size_t b64size = 0;
    output = digest_sha1(input, strlen((char *)input), &outsize);
    assert( output != NULL );
    printf("the base64 sha1 hash of '%s' is '%s'\n",
        input, mbase64_encode((const unsigned char *)output, strlen((char *)output)));
    printf("the hexdigest of the sha1 hash is %s\n", tohex(output, outsize));
    return 0;
}

int
main(void) {
    return test_sha1_hexdigest();
}
