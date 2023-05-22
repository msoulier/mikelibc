#ifndef MNET_H
#define MNET_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TCP = 1,
    UDP,
    EITHER
} mSockType;

/*
 * Run getaddrinfo on the host and service provided.
 * 0 good, non-zero bad.
 */
int maddrlookup(const char *host, const char *service, mSockType socktype);

/*
 * Set up a UDP server. Return the socket fd, or 0 on error.
 */
int
setup_udp_server(char *address, int port);

/*
 * Given a listening udp socket, read a datagram and return it. The
 * dgram char* must be supplied by the caller. It will be populated, and
 * also returned, returning NULL on error.
 */
char *
read_dgram(int socketfd, char *dgram, size_t dgram_size);

/*
 * Set up a TCP server. Return the socket fd, or 0 on error.
 */
int
setup_tcp_server(char *address, int port, int backlog);

/*
 * Connect to a TCP server. Return the socket fd, or -1 on error.
 */
int
connect_tcp_client(const char *address, const char *port);

#ifdef __cplusplus
};
#endif

#endif
