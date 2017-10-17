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
	Connection(int);
	Connection(int, int, int);

	void setTimeout(int, int);

	ssize_t receives(struct sockaddr_in *, char *, int *);

	ssize_t sends(struct sockaddr_in *, const char *, int *);

private:
	int socket;
	struct timeval timeout;
};

#endif
