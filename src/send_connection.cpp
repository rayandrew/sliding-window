#include "send_connection.h"
#include "packet.h"
#include "utils.h"
#include <algorithm>

SendConnection::SendConnection(const char *host, const char *port) : sock(host, port) {
	sendWindowSize = 256;
	advBytes = 256;
	ackTimeout = 2 * MICROSECONDS_IN_A_SECOND;

	nextAckSeq = 0;
	nextSentSeq = 0;
	newSeq = 0;

	nextBufferItemToSend = buffer.begin();
}

int SendConnection::send_data(unsigned char *message, unsigned int messageSize) {
	sock.setRecvTimeout(ackTimeout);

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
			Packet packet(nextBufferItemToSend->data, nextBufferItemToSend->seq);
			if (sock.socketSend(packet.bytes(), Packet::SIZE) <= 0) {
				log_error("Failed to send packet (seq: " + toStr(nextBufferItemToSend->seq) + ")");
			}
			nextBufferItemToSend->timestamp = timer();
			log_info("Sent packet (seq: " + toStr(nextBufferItemToSend->seq) + ", data: " + toStr(nextBufferItemToSend->data) + ", timer: " + toStr(nextBufferItemToSend->timestamp) + ")");
			nextSentSeq++;
			nextBufferItemToSend++;
		}

		/* Listen for ACK */
		log_debug("Listening for ACK...");
		unsigned char ackMessage[AckPacket::SIZE];
		int recvSize = sock.socketRecv(ackMessage, AckPacket::SIZE);
		if (recvSize > 0) {
			AckPacket ackPacket(ackMessage);
			if (ackPacket.isValid()) {
				unsigned int receivedNextAckSeq = ackPacket.getNextSeq();
				if ((receivedNextAckSeq - nextAckSeq) < (nextSentSeq - nextAckSeq)) {
					nextAckSeq = receivedNextAckSeq;
					log_info("Received ACK (nextSeq: " + toStr(nextAckSeq) + ", adv: " + toStr(ackPacket.getAdv()) + ")");
					while (!buffer.empty() && buffer.front().seq != nextAckSeq) {
						buffer.pop_front();
						messageBytesAcked++;
					}
					advBytes = ackPacket.getAdv();
				} else {
					log_info("Rejected ACK outside window (nextSeq: " + toStr(receivedNextAckSeq) + ")");
				}
			} else {
				log_info("Rejected ACK with invalid checksum");
			}
		}

		// TODO: also resend if receiving duplicate ACKs for previous packet

		/* Update packet timeouts, resend timeout packets */
		unsigned long currentTimestamp = timer();
		std::deque<SendBufferItem>::iterator nextBufferItemToCheck = buffer.begin();
		unsigned int packetsAwaitingAck = nextSentSeq - nextAckSeq;
		log_debug("Updating timeout... (timestamp: " + toStr(currentTimestamp) + ")");
		log_debug("(nextSentSeq: " + toStr(nextSentSeq) + ", nextAckSeq: " + toStr(nextAckSeq) + ")");
		for (unsigned int i = 0; i < packetsAwaitingAck; i++) {
			if (currentTimestamp - nextBufferItemToCheck->timestamp >= ackTimeout*MICROSECONDS_IN_A_NANOSECOND) {
				Packet packet(nextBufferItemToCheck->data, nextBufferItemToCheck->seq);
				if (sock.socketSend(packet.bytes(), Packet::SIZE) <= 0) {
					log_error("Failed to resend packet (seq: " + toStr(nextBufferItemToCheck->seq) + ")");
				}
				nextBufferItemToCheck->timestamp = timer();
				log_info("ACK timeout, packet resent (seq: " + toStr(nextBufferItemToCheck->seq) + ")");
			}
			nextBufferItemToCheck++;
		}

		// TODO: handle adv == 0 case
	}

	return messageBytesAcked;
}