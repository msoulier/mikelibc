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
#include "mlogger.h"

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

int
setup_udp_server(char *address, int port) {
    struct sockaddr_in address_inet;
    int inet_length;
    int socketfd;
    int rv;
    int rcvbuf = 262144; // 2^18
    socklen_t option_size;
    int reuseaddr = 1;

    logmsg(DEBUG, "setup_network: creating datagram socket");
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0)
        return 0;

    // Set up some socket options.
    logmsg(DEBUG, "Setting SO_REUSEADDR to true");
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));

    logmsg(DEBUG, "Setting SO_RCVBUF to %d bytes", rcvbuf);
    if (setsockopt(socketfd,
                   SOL_SOCKET,
                   SO_RCVBUF,
                   &rcvbuf, 
                   sizeof(rcvbuf)) < 0)
    {
        logmsg(WARNING, "Failed to set SO_RCVBUF: %s",
                strerror(errno));
    }
    else
    {
        option_size = sizeof(int);
        if (getsockopt(socketfd,
                       SOL_SOCKET,
                       SO_RCVBUF,
                       &rcvbuf,
                       &option_size) < 0)
        {
            logmsg(WARNING, "getsockopt failed: %s",
                    strerror(errno));
        }
        else
        {
            logmsg(DEBUG, "SO_RCVBUF is now %d bytes", rcvbuf);
        }
    }

    memset(&address_inet, 0, sizeof(address_inet));
    address_inet.sin_family = AF_INET;
    address_inet.sin_port = htons(port);
    address_inet.sin_addr.s_addr = inet_addr(address);

    if (address_inet.sin_addr.s_addr == INADDR_NONE)
        return 0;

    inet_length = sizeof(address_inet);

    logmsg(DEBUG, "binding to %s:%d", address, port);
    rv = bind(socketfd, (struct sockaddr *)&address_inet, inet_length);
    if (rv < 0)
        return 0;

    return socketfd;
}

char *
read_dgram(int socketfd, char *dgram, size_t dgram_size) {
    struct sockaddr_in address_client;
    socklen_t inet_length;
    int rv;

    // NULL out the buffer.
    memset(dgram, '\0', dgram_size);

    // Check socket for data.
    logmsg(DEBUG, "blocking on recvfrom...");
    inet_length = sizeof(address_client);
    rv = recvfrom(socketfd,
                  dgram,
                  dgram_size,
                  0,
                  (struct sockaddr *)&address_client,
                  &inet_length);
    logmsg(INFO, "returned from recvfrom, rv is %d", rv);
    // FIXME - should we screen the size of the message here?
    if (rv == 0) {
        logmsg(WARNING, "Read 0 bytes from socket. Not sure why.");
        return NULL;
    } else if (rv < 0) {
        logmsg(WARNING, "Error on socket: %s", strerror(errno));
        return NULL;
    }
    return dgram;
}
