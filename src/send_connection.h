#ifndef SW_SEND_CONNECTION_H
#define SW_SEND_CONNECTION_H

#include <deque>
#include "socket.h"

class SendConnection {
public:
	SendConnection(const char *host, const char *port);

	int send_data(unsigned char *message, unsigned int messageSize);

	void setSendWindowSize(unsigned int sendWindowSize) {
		this->sendWindowSize = sendWindowSize;
	}

	void setAdvertisedWindowSize(unsigned int adv) {
		this->advBytes = adv;
	}

	void setAckTimeout(unsigned long us) {
		this->ackTimeout = us;
	}

	bool isValid() {
	    return sock.isValid();
	}

private:

	typedef struct {
		unsigned int seq;
		unsigned char data;
		unsigned long timestamp;
	} SendBufferItem;

	Socket sock;

	unsigned int sendWindowSize;
	unsigned int advBytes;
	unsigned long ackTimeout;

	unsigned int nextAckSeq;
	unsigned int nextSentSeq;
	unsigned int newSeq;

	std::deque<SendBufferItem> buffer;
	unsigned int nextBufferItemToSendIndex;
};

#endif
