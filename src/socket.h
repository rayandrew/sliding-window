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
	int socketSend(const unsigned char *data, unsigned int len);

	bool isValid();

private:
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	struct timeval recvTimeout;
	bool valid;
};

#endif
