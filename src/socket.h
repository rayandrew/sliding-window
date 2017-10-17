#ifndef SW_SOCKET_H
#define SW_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class Socket {
public:
	Socket(const char *host, const char *port, bool listen);
	~Socket();

	void setRecvTimeout(unsigned long ns);

	int socketRecv(unsigned char *data, unsigned int len);
	int socketRecvFrom(unsigned char *data, unsigned int len, struct sockaddr *srcAddress, socklen_t *srcAddressLength);
	int socketSend(const unsigned char *data, unsigned int len);
	int socketSendTo(const unsigned char *data, unsigned int len, const struct sockaddr *dest, socklen_t destAddressLength);

	bool isValid();

private:
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	struct timeval recvTimeout;
	bool valid;
};

#endif
