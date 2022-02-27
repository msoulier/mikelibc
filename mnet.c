#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "mnet.h"
#include "mdebug.h"

void paddr_error(int err) {
    fprintf(stderr, "maddr: ");
    switch (err) {
        case EAI_NONAME:
            fprintf(stderr, "Name or service not known.\n");
            break;
        case EAI_AGAIN:
            fprintf(stderr, "Temporary error.\n");
            break;
        case EAI_FAIL:
            fprintf(stderr, "Permanent failure.\n");
            break;
        case EAI_SOCKTYPE:
            fprintf(stderr, "Requested socket type not supported.\n");
            break;
        case EAI_SERVICE:
            fprintf(stderr, "Requested service not available.\n");
            break;
        case EAI_SYSTEM:
            perror("System error");
            break;
        default:
            fprintf(stderr, "Something bad happened. Not sure what.\n");
    }
}

int maddrlookup_tcp(const char *host, const char *service) {
    struct addrinfo *infop = NULL;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // AF_UNSPEC for v4 or v6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    mdebugf("maddrlookup_tcp: host %s, service %s\n", host, service);

    int rv;
    if ((rv = getaddrinfo(host, service, &hints, &infop)) != 0) {
        paddr_error(rv);
        return rv;
    }
    mdebugf("getaddrinfo rv was %d\n", rv);
    for ( ; infop != NULL; infop = infop->ai_next) {
        struct sockaddr_in *sa = (struct sockaddr_in *)infop->ai_addr;
        printf("\n%s port: %d protocol: %d\n",
           inet_ntoa(sa->sin_addr),
           ntohs(sa->sin_port),
           infop->ai_protocol);
    }
    return 0;
}
