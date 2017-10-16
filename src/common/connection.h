#ifndef SW_CONNECTION_H
#define SW_CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class Connection {
public:
	Connection(int, struct sockaddr_in *);
	Connection(int, struct sockaddr_in *, int = 0, int = 0);

	void setTimeout(int, int);

	int recvfrom(char *, int *);

	int sendto();

private:
	int socket;
	struct sockaddr_in *connection;
	struct timeval timeout;
};

#endif
