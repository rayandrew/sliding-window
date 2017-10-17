#include "connection.h"

Connection::Connection(const char *host, const char *port) {
    memset(&this->hints, 0, sizeof(this->hints));
    this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_DGRAM;

    int rv;
    if ((rv = getaddrinfo(host, port, &this->hints, &this->servinfo)) != 0) {
        //fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        this->isValid = false;
    }

    /* Loop through all the results and make a socket */
    for (this->p = this->servinfo; this->p != NULL; this->p = this->p->ai_next) {
        if ((this->socket = socket(this->p->ai_family, this->p->ai_socktype, this->p->ai_protocol)) == -1) {
            this->isValid = false;
            continue;
        }
        break;
    }
    if (this->p == NULL) {
        this->isValid = false;
        //fprintf(stderr, "talker: failed to create socket\n");
    }
}

void Connection::setRecvTimeout(unsigned long us)
{
    this->recvTimeout.tv_sec = us / MICROSECONDS_IN_A_SECOND;
    this->recvTimeout.tv_usec = us % MICROSECONDS_IN_A_SECOND;
}

int recv(char *data, unsigned int len) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(this->socket, &fds);

    int fromlen;

    int checkTimeout = select(this->socket + 1, &fds, NULL, NULL, &this->recvTimeout);
    if (n == 0) {
        // timeout
        return -2;
    } else if (n == -1) {
        // error
        return -1;
    } else {
        return recv(this->socket, data, len, 0)
    }

    return (checkTimeout == 0 || checkTimeout == -1) ? 0 : recvfrom(socket, buf, *length, 0, (struct sockaddr *)connection, (socklen_t *)&fromlen);
}

int send(const char *data, unsigned int len) {
    int numbytes;
    if ((numbytes = sendto(this->socket, data, len, 0, this->p->ai_addr, this->p->ai_addrlen)) == -1) {
        //perror("talker: sendto");
    }
    return numbytes;
}
