#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "mnet.h"
#include "mdebug.h"

int test_tcp_client(void) {
    int sockfd = connect_tcp_client("duckduckgo.com", "80");
    assert( sockfd > 0 );
    if (sockfd > 0) {
        mdbgf("successfully connected via TCP to port 80 on duckduckgo.com\n");
    }
}

int
main(void) {
    return test_tcp_client();
}
