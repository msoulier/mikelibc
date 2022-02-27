#ifndef MNET_H
#define MNET_H

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

#endif
