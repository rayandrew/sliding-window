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
unsigned long ackTimeout = 2 * MICROSECONDS_IN_A_SECOND;

unsigned int nextAckSeq = 0;
unsigned int nextSentSeq = 0;
unsigned int newSeq = 0;

std::deque<SendBufferItem> buffer;
std::deque<SendBufferItem>::iterator nextBufferItemToSend = buffer.begin();

void init_tx() { // TODO
	sendWindowSize = 256;
	advBytes = 256;
}

int send_data(Connection &conn, unsigned char *message, unsigned int messageSize) {
	conn.setRecvTimeout(ackTimeout);

	unsigned int messageBytesAcked = 0;
	unsigned int messageBytesProcessed = 0;
	while (messageBytesAcked < messageSize) {

		/* Refill the buffer */
		unsigned int bytesToRefill = std::min(sendWindowSize - (newSeq - nextAckSeq), messageSize - messageBytesProcessed);
		log_debug("Refilling buffer (startSeq: " + toStr(newSeq) + ", amount: " + toStr(bytesToRefill) + ")");
		for (unsigned int i = 0; i < bytesToRefill; i++) {
			SendBufferItem sbi;
			sbi.seq = newSeq;
			sbi.data = message[i];
			buffer.push_back(sbi);
			newSeq++;
		}
		messageBytesProcessed += bytesToRefill;

		/* Send bytes as allowed by the advertised window size */
		log_debug("Sending from seq: " + toStr(newSeq));
		unsigned int bytesToSend = std::min(advBytes, newSeq - nextSentSeq);
		for (unsigned int i = 0; i < bytesToSend; i++) {
			Frame frame(nextBufferItemToSend->data, nextBufferItemToSend->seq);
			if (conn.tx(frame.bytes(), Frame::SIZE) <= 0) {
				log_error("Failed to send packet (seq: " + toStr(nextBufferItemToSend->seq) + ")");
			}
			nextBufferItemToSend->timestamp = timer();
			log_info("Sent packet (seq: " + toStr(nextBufferItemToSend->seq) + ", data: " + toStr(nextBufferItemToSend->data) + ", timer: " + toStr(nextBufferItemToSend->timestamp) + ")");
			nextSentSeq++;
			nextBufferItemToSend++;
		}

		/* Listen for ACK */
		log_debug("Listening for ACK...");
		unsigned char ackMessage[AckFrame::SIZE];
		int recvSize = conn.rx(ackMessage, AckFrame::SIZE);
		if (recvSize > 0) {
			AckFrame ackFrame(ackMessage);
			if (ackFrame.isValid()) {
				unsigned int receivedNextAckSeq = ackFrame.getNextSeq();
				if ((receivedNextAckSeq - nextAckSeq) < (nextSentSeq - nextAckSeq)) {
					nextAckSeq = receivedNextAckSeq;
					log_info("Received ACK (nextSeq: " + toStr(nextAckSeq) + ", adv: " + toStr(ackFrame.getAdv()) + ")");
					while (!buffer.empty() && buffer.front().seq != nextAckSeq) {
						buffer.pop_front();
						messageBytesAcked++;
					}
					advBytes = ackFrame.getAdv();
				} else {
					log_info("Rejected ACK outside window (nextSeq: " + toStr(receivedNextAckSeq) + ")");
				}
			} else {
				log_info("Rejected ACK with invalid checksum");
			}
		}

		// TODO: also resend if receiving duplicate ACKs for previous frame

		/* Update frame timeouts, resend timeout frames */
		unsigned long currentTimestamp = timer();
		std::deque<SendBufferItem>::iterator nextBufferItemToCheck = buffer.begin();
		unsigned int framesAwaitingAck = nextSentSeq - nextAckSeq;
		log_debug("Updating timeout... (timestamp: " + toStr(currentTimestamp) + ")");
		log_debug("(nextSentSeq: " + toStr(nextSentSeq) + ", nextAckSeq: " + toStr(nextAckSeq) + ")");
		for (unsigned int i = 0; i < framesAwaitingAck; i++) {
			if (currentTimestamp - nextBufferItemToCheck->timestamp >= ackTimeout*MICROSECONDS_IN_A_NANOSECOND) {
				Frame frame(nextBufferItemToCheck->data, nextBufferItemToCheck->seq);
				if (conn.tx(frame.bytes(), Frame::SIZE) <= 0) {
					log_error("Failed to resend packet (seq: " + toStr(nextBufferItemToCheck->seq) + ")");
				}
				nextBufferItemToCheck->timestamp = timer();
				log_info("ACK timeout, packet resent (seq: " + toStr(nextBufferItemToCheck->seq) + ")");
			}
			nextBufferItemToCheck++;
		}
	}

	return 0;
}
