#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "mnet.h"
#include "mdebug.h"

int maddrlookup_tcp(const char *host, const char *service) {
    struct addrinfo *infop = NULL;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // AF_UNSPEC for v4 or v6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    mdebugf("maddrlookup_tcp: host %s, service %s\n");

    int rv;
    if ((rv = getaddrinfo(host, service, &hints, &infop)) > 0) {
        fprintf(stderr, "failed to lookup host: %d\n", rv);
        return rv;
    }
    for ( ; infop != NULL; infop = infop->ai_next) {
        struct sockaddr_in *sa = (struct sockaddr_in *)infop->ai_addr;
        printf("\n%s port: %d protocol: %d\n",
           inet_ntoa(sa->sin_addr),
           ntohs(sa->sin_port),
           infop->ai_protocol);
    }
    return 0;
}
