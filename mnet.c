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

int maddrlookup(const char *host, const char *service, mSockType socktype) {
    int rv;
    struct addrinfo *infop = NULL;
    struct addrinfo *current = NULL;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // AF_UNSPEC for v4 or v6
    hints.ai_flags = AI_PASSIVE;
    if (socktype == TCP) {
        hints.ai_socktype = SOCK_STREAM;
    } else if (socktype == UDP) {
        hints.ai_socktype = SOCK_DGRAM;
    }

    mdebugf("maddrlookup: host %s, service %s\n", host, service);

    if ((rv = getaddrinfo(host, service, &hints, &infop)) != 0) {
        paddr_error(rv);
        goto CLEANUP;
    }
    mdebugf("getaddrinfo rv was %d\n", rv);
    for (current = infop; current != NULL; current = current->ai_next) {
        struct sockaddr_in *sa = (struct sockaddr_in *)current->ai_addr;
        printf("\n%s port: %d protocol: %d\n",
           inet_ntoa(sa->sin_addr),
           ntohs(sa->sin_port),
           current->ai_protocol);
    }
CLEANUP:
    mdebugf("CLEANUP: calling freeaddrinfo\n");
    if (infop != NULL)
        freeaddrinfo(infop);
    return rv;
}
