#ifndef SW_RECV_CONNECTION_H
#define SW_RECV_CONNECTION_H

#include <map>
#include "socket.h"

class RecvConnection {
public:
	RecvConnection(const char *host, const char *port);

	int recv_data(unsigned char *message, unsigned int messageSize);

	void setReceiveWindowSize(unsigned int receiveWindowSize) {
		this->receiveWindowSize = receiveWindowSize;
	}

	void setReceiveTimeout(unsigned long us) {
		this->recvTimeout = us;
	}

	bool isValid() {
	    return sock.isValid();
	}

private:
	Socket sock;

	unsigned int receiveWindowSize;
	unsigned long recvTimeout;

	unsigned int nextValidatedSeq;
	unsigned int nextRecvSeq;

	struct sockaddr_in lastReceivedAddress;
	socklen_t lastReceivedAddressLength;

	std::map<unsigned int, unsigned char> buffer;
};

#endif
