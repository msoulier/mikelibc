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
#include "mlog.h"

int maddrlookup(const char *host, const char *service, mSockType socktype) {
    int rv;
    struct addrinfo *infop = NULL;
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
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        goto CLEANUP;
    }
    mdebugf("getaddrinfo rv was %d\n", rv);
#ifdef MDEBUG
    struct addrinfo *current = NULL;
    for (current = infop; current != NULL; current = current->ai_next) {
        struct sockaddr_in *sa = (struct sockaddr_in *)current->ai_addr;
        mdebugf("\n%s port: %d protocol: %d\n",
           inet_ntoa(sa->sin_addr),
           ntohs(sa->sin_port),
           current->ai_protocol);
    }
#endif
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

    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0)
        return 0;

    // Set up some socket options.
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));

    if (setsockopt(socketfd,
                   SOL_SOCKET,
                   SO_RCVBUF,
                   &rcvbuf, 
                   sizeof(rcvbuf)) < 0)
    {
        fprintf(stderr, "Failed to set SO_RCVBUF: %s", strerror(errno));
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
            fprintf(stderr, "getsockopt failed: %s", strerror(errno));
        }
    }

    memset(&address_inet, 0, sizeof(address_inet));
    address_inet.sin_family = AF_INET;
    address_inet.sin_port = htons(port);
    address_inet.sin_addr.s_addr = inet_addr(address);

    if (address_inet.sin_addr.s_addr == INADDR_NONE)
        return 0;

    inet_length = sizeof(address_inet);

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
    inet_length = sizeof(address_client);
    rv = recvfrom(socketfd,
                  dgram,
                  dgram_size,
                  0,
                  (struct sockaddr *)&address_client,
                  &inet_length);
    // FIXME - should we screen the size of the message here?
    if (rv == 0) {
        return NULL;
    } else if (rv < 0) {
        fprintf(stderr, "Error on socket: %s", strerror(errno));
        return NULL;
    }
    return dgram;
}

int
setup_tcp_server(char *address, int port, int backlog) {
    struct sockaddr_in address_inet;
    int inet_length;
    int socketfd;
    int rv;
    int rcvbuf = 262144; // 2^18
    socklen_t option_size;
    int reuseaddr = 1;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("socket");
        return 0;
    }

    // Set up some socket options.
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));

    if (setsockopt(socketfd,
                   SOL_SOCKET,
                   SO_RCVBUF,
                   &rcvbuf, 
                   sizeof(rcvbuf)) < 0)
    {
        fprintf(stderr, "Failed to set SO_RCVBUF: %s", strerror(errno));
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
            fprintf(stderr, "getsockopt failed: %s", strerror(errno));
        }
    }

    memset(&address_inet, 0, sizeof(address_inet));
    address_inet.sin_family = AF_INET;
    address_inet.sin_port = htons(port);
    address_inet.sin_addr.s_addr = inet_addr(address);

    if (address_inet.sin_addr.s_addr == INADDR_NONE)
        return 0;

    inet_length = sizeof(address_inet);

    rv = bind(socketfd, (struct sockaddr *)&address_inet, inet_length);
    if (rv < 0) {
        perror("bind");
        return 0;
    }

    if (listen(socketfd, backlog) != 0) {
        perror("listen");
        return 0;
    }

    return socketfd;
}

int
connect_tcp_client(const char *address, const char *port) {
    mdebugf("connect_tcp_client - address %s, port %s", address, port);
    struct addrinfo hints;
    struct addrinfo *infop = NULL;
    struct addrinfo *current = NULL;
    int clientfd, rv;
    struct sockaddr_in clientsock;
    socklen_t clientsock_len = sizeof(struct sockaddr_in);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // AF_UNSPEC for v4 or v6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP;

    if ((rv = getaddrinfo(address, port, &hints, &infop)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        rv = -1;
        goto CLEANUP;
    }
    for (current = infop; current != NULL; current = current->ai_next) {
        struct sockaddr_in *sa = (struct sockaddr_in *)current->ai_addr;
        mdebugf("\n%s port: %d protocol: %d\n",
                inet_ntoa(sa->sin_addr),
                ntohs(sa->sin_port),
                current->ai_protocol);

        clientfd = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
        if (clientfd == -1) {
            fprintf(stderr, "socket creation failed: %s", strerror(errno));
            continue;
        }
        if (connect(clientfd, current->ai_addr, current->ai_addrlen) < 0) {
            perror("connect");
            rv = -1;
            goto CLEANUP;
        }
        if (getsockname(clientfd, (struct sockaddr*)&clientsock, &clientsock_len) < 0) {
        }
        else {
            mdebugf("local port is %d", ntohs(clientsock.sin_port));
        }
        // If we're here we connected.
        rv = clientfd;
        break;
    }
    if (current == NULL) {
        fprintf(stderr, "failed to connect to all resolved hosts");
        rv = -1;
        goto CLEANUP;
    }
CLEANUP:
    if (infop != NULL)
        freeaddrinfo(infop);
    return rv;
}
