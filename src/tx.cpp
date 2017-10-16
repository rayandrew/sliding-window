#include "tx.h"
#include "common/frame.h"
#include "common/utils.h"
#include <algorithm>
#include <deque>
#include <cstring>

typedef struct {
	unsigned int seq;
	unsigned char data;
	unsigned long timestamp;
} SendBufferItem;

unsigned int sendWindowSize = 256;
unsigned int advBytes = 256;
unsigned long ackTimeout = 2000000000;

unsigned int nextAckSeq = 0;
unsigned int nextSentSeq = 0;
unsigned int newSeq = 0;

std::deque<SendBufferItem> buffer;
std::deque<SendBufferItem>::iterator nextBufferItemToSend = buffer.begin();

void init_tx() {
	sendWindowSize = 256;
	advBytes = 256;
}

unsigned int send_data(int socket, unsigned char *message, unsigned int messageSize, int flags, const struct sockaddr *destAddress, socklen_t addressLength) {
	unsigned int messageBytesProcessed = 0;
	while (messageBytesProcessed < messageSize) {

		/* Refill the buffer */
		unsigned int bytesToRefill = std::min(sendWindowSize - (newSeq - nextAckSeq), messageSize - messageBytesProcessed);
		for (unsigned int i = 0; i < bytesToRefill; i++) {
			SendBufferItem sbi;
			sbi.seq = newSeq;
			sbi.data = message[i];
			buffer.push_back(sbi);
			newSeq++;
		}

		/* Send bytes as allowed by the advertised window size */
		unsigned int bytesToSend = std::min(advBytes, newSeq - nextSentSeq);
		for (unsigned int i = 0; i < bytesToSend; i++) {
			Frame frame(nextBufferItemToSend->data, nextBufferItemToSend->seq);
			sendto(socket, frame.bytes(), Frame::SIZE, flags, destAddress, addressLength); // TODO: proper networking
			nextBufferItemToSend->timestamp = timer();
			nextSentSeq++;
			nextBufferItemToSend++;
		}

		/* Listen for ACK */
		unsigned char ackMessage[AckFrame::SIZE];
		int recvSize = recvfrom(socket, ackMessage, AckFrame::SIZE, flags, destAddress, addressLength); // TODO: timer ~ frame timeout
		if (recvSize > 0) {
			AckFrame ackFrame(ackMessage);
			if (ackFrame.isValid()) {
				unsigned int receivedNextAckSeq = ackFrame.getNextSeq();
				if ((receivedNextAckSeq - nextAckSeq) < (nextSentSeq - nextAckSeq)) {
					nextAckSeq = receivedNextAckSeq;
					while (!buffer.empty() && buffer.front().seq != nextAckSeq) {
						buffer.pop_front();
						messageBytesProcessed++;
					}
					advBytes = ackFrame.getAdv();
				}
			}
		}

		// TODO: also resend if receiving duplicate ACKs for previous frame

		/* Update frame timeouts, resend timeout frames */
		std::deque<SendBufferItem>::iterator nextBufferItemToCheck = buffer.begin();
		unsigned int framesAwaitingAck = nextSentSeq - nextAckSeq;
		for (unsigned int i = 0; i < framesAwaitingAck; i++) {
			if (timer() - nextBufferItemToCheck->timestamp >= ackTimeout) {
				Frame frame(nextBufferItemToCheck->data, nextBufferItemToCheck->seq);
				sendto(socket, frame.bytes, Frame::SIZE, flags, destAddress, addressLength);
				nextBufferItemToCheck->timestamp = timer();
			}
			nextBufferItemToCheck++;
		}
	}

	return 0;
}
