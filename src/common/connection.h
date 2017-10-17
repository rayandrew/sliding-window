#ifndef SW_CONNECTION_H
#define SW_CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class Connection {
public:
	Connection(const char *host, const char *port);
	~Connection();

	void setRecvTimeout(unsigned long ns);

	int rx(unsigned char *data, unsigned int len);
	int tx(const unsigned char *data, unsigned int len);

	bool isValid();

private:
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	struct timeval recvTimeout;
	bool valid;
};

#endif
