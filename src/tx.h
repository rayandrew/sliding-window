#ifndef SW_TX_H
#define SW_TX_H

#include "common/network.h"

unsigned int send_data(int sockfd, unsigned char *msg, unsigned int size, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

#endif
