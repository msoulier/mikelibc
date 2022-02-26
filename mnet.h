#ifndef MNET_H
#define MNET_H

/*
 * Run getaddrinfo on the host and service provided.
 * 0 good, non-zero bad.
 */
int maddrlookup_tcp(const char *host, const char *service);

#endif
