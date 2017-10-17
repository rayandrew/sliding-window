#ifndef SW_CONNECTION_H
#define SW_CONNECTION_H

#define MICROSECONDS_IN_A_SECOND 1000000

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

	int recv(char *data, unsigned int len);
	int send(const char *data, unsigned int len);

private:
	int socket;
	struct addrinfo hints, *servinfo, *p;
	struct timeval recvTimeout;
	bool isValid;
};

#endif
