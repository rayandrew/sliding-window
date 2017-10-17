#include "socket.h"
#include "utils.h"
#include <cstring>

/* Create a new socket. To create a socket that listens to all addresses, use NULL as host. */
Socket::Socket(const char *host, const char *port, bool listen) {
    memset(&this->hints, 0, sizeof(this->hints));
    this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_DGRAM;
    if (host == NULL) this->hints.ai_flags = AI_PASSIVE;

    /* Enable socket address reuse */
    int reuseAddressValue = 1;
    setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseAddressValue, sizeof reuseAddressValue);

    int rv;
    if ((rv = getaddrinfo(host, port, &this->hints, &this->servinfo)) != 0) {
        log_error("Socket error: getaddrinfo: " + toStr(gai_strerror(rv)));
        this->valid = false;
    }

    /* Loop through all the result addresses and make a socket using the first one */
    for (this->p = this->servinfo; this->p != NULL; this->p = this->p->ai_next) {
        this->sockfd = socket(this->p->ai_family, this->p->ai_socktype, this->p->ai_protocol);
        if (this->sockfd == -1) {
            this->valid = false;
            continue;
        } else if (listen) { /* If this is a listening socket, bind to port */
            if (bind(this->sockfd, this->p->ai_addr, this->p->ai_addrlen) == -1) {
                close(this->sockfd);
                this->valid = false;
                continue;
            }
            log_debug("Socket: listening to " + toStr(host) + ":" + toStr(port) + "...");
        }

        this->valid = true;
        break;
    }
    if (this->p == NULL) {
        this->valid = false;
    }
    if (!this->valid) {
        log_error("Socket error: failed to create socket");
    }
}

Socket::~Socket() {
    freeaddrinfo(this->servinfo);
    close(this->sockfd);
}

void Socket::setRecvTimeout(unsigned long us)
{
    this->recvTimeout.tv_sec = us / MICROSECONDS_IN_A_SECOND;
    this->recvTimeout.tv_usec = us % MICROSECONDS_IN_A_SECOND;
}

int Socket::socketRecv(unsigned char *data, unsigned int len) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(this->sockfd, &fds);

    struct timeval timeout = this->recvTimeout;
    int checkTimeout = select(this->sockfd + 1, &fds, NULL, NULL, &timeout);
    if (checkTimeout == 0) {
        // timeout
        return -2;
    } else if (checkTimeout == -1) {
        log_error("Socket error: socketRecv: select failed");
        return -1;
    } else {
        return recv(this->sockfd, data, len, 0);
    }
}

int Socket::socketRecvFrom(unsigned char *data, unsigned int len, struct sockaddr *srcAddress, socklen_t *srcAddressLength) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(this->sockfd, &fds);

    struct timeval timeout = this->recvTimeout;
    int checkTimeout = select(this->sockfd + 1, &fds, NULL, NULL, &timeout);
    if (checkTimeout == 0) {
        // timeout
        return -2;
    } else if (checkTimeout == -1) {
        log_error("Socket error: socketRecvFrom: select failed");
        return -1;
    } else {
        return recvfrom(this->sockfd, data, len, 0, srcAddress, srcAddressLength);
    }
}

/* Will send to this socket's specified host and port */
int Socket::socketSend(const unsigned char *data, unsigned int len) {
    return socketSendTo(data, len, this->p->ai_addr, this->p->ai_addrlen);
}

int Socket::socketSendTo(const unsigned char *data, unsigned int len, const struct sockaddr *dest, socklen_t destAddressLength) {
    int numbytes = sendto(this->sockfd, data, len, 0, dest, destAddressLength);
    if (numbytes == -1) {
        log_error("Socket error: sendto failed");
    }
    return numbytes;
}

bool Socket::isValid() {
    return this->valid;
}

void* Socket::getInAddress(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}