#include "connection.h"
#include "utils.h"
#include <cstring>

Connection::Connection(const char *host, const char *port) {
    memset(&this->hints, 0, sizeof(this->hints));
    this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_DGRAM;

    int rv;
    if ((rv = getaddrinfo(host, port, &this->hints, &this->servinfo)) != 0) {
        //fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        this->valid = false;
    }

    /* Loop through all the results and make a socket */
    for (this->p = this->servinfo; this->p != NULL; this->p = this->p->ai_next) {
        this->sockfd = socket(this->p->ai_family, this->p->ai_socktype, this->p->ai_protocol);
        if (this->sockfd == -1) {
            this->valid = false;
            continue;
        }
        break;
    }
    if (this->p == NULL) {
        this->valid = false;
        //fprintf(stderr, "talker: failed to create socket\n");
    }
}

Connection::~Connection() {
    freeaddrinfo(this->servinfo);
    close(this->sockfd);
}

void Connection::setRecvTimeout(unsigned long us)
{
    this->recvTimeout.tv_sec = us / MICROSECONDS_IN_A_SECOND;
    this->recvTimeout.tv_usec = us % MICROSECONDS_IN_A_SECOND;
}

int Connection::rx(unsigned char *data, unsigned int len) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(this->sockfd, &fds);

    struct timeval timeout = this->recvTimeout;
    int checkTimeout = select(this->sockfd + 1, &fds, NULL, NULL, &timeout);
    if (checkTimeout == 0) {
        // timeout
        return -2;
    } else if (checkTimeout == -1) {
        // error
        return -1;
    } else {
        return recv(this->sockfd, data, len, 0);
    }
}

int Connection::tx(const unsigned char *data, unsigned int len) {
    int numbytes;
    if ((numbytes = sendto(this->sockfd, data, len, 0, this->p->ai_addr, this->p->ai_addrlen)) == -1) {
        //perror("talker: sendto");
    }
    return numbytes;
}

bool Connection::isValid() {
    return this->valid;
}