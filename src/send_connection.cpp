#include "send_connection.h"
#include "packet.h"
#include "utils.h"
#include <algorithm>

SendConnection::SendConnection(const char *host, const char *port) : sock(host, port, false) {
	sendWindowSize = 256;
	advBytes = 256;
	ackTimeout = 2 * MICROSECONDS_IN_A_SECOND;

	nextAckSeq = 0;
	nextSentSeq = 0;
	newSeq = 0;

	nextBufferItemToSendIndex = 0;
}

int SendConnection::sendEndOfTransmission() {
	log_info("Sending end-of-transmission packet...");
	//sock.setRecvTimeout(ackTimeout);

	/* Send EOT until an EOT_ACK reply is received */
	log_debug("Listening for end-of-transmission reply...");
	unsigned char ackMessage[AckPacket::SIZE];
	while (true) {
		Packet packet(0, newSeq);
		packet.setEndOfTransmission();
		if (sock.socketSend(packet.bytes(), Packet::SIZE) <= 0) {
			log_error("Failed to send EOT (eotSeq: " + toStr(packet.getSeq()) + ")");
		}

		int recvSize = sock.socketRecv(ackMessage, AckPacket::SIZE);
		if (recvSize > 0) {
			AckPacket ackPacket(ackMessage);
			if (ackPacket.isValid() && ackPacket.isEndOfTransmission() && ackPacket.getNextSeq() == newSeq) {
				log_info("Received EOT_ACK packet (eotSeq: " + toStr(ackPacket.getNextSeq()) + ")");
				return 1;
			} else {
				log_info("Rejected invalid or non-EOT_ACK packet.");
			}
		}
	}

	return 0;
}

int SendConnection::send_data(unsigned char *message, unsigned int messageSize) {
	log_info("Sending message (size: " + toStr(messageSize) + " bytes)...");
	sock.setRecvTimeout(ackTimeout);

	unsigned long currentTimestamp;
	unsigned int messageBytesAcked = 0;
	unsigned int messageBytesProcessed = 0;
	while (messageBytesAcked < messageSize) {

		/* Refill the buffer */
		unsigned int bytesToRefill = std::min(sendWindowSize - (newSeq - nextAckSeq), messageSize - messageBytesProcessed);
		log_debug("Refilling buffer (startSeq: " + toStr(newSeq) + ", amount: " + toStr(bytesToRefill) + ")");
		for (unsigned int i = 0; i < bytesToRefill; i++) {
			SendBufferItem sbi;
			sbi.seq = newSeq;
			sbi.data = message[messageBytesProcessed + i];
			buffer.push_back(sbi);
			newSeq++;
		}
		messageBytesProcessed += bytesToRefill;

		/* Send bytes as allowed by the advertised window size */
		log_debug("Last seq in buffer: " + toStr(newSeq-1));
		unsigned int bytesToSend = std::min(advBytes, newSeq - nextSentSeq);
		currentTimestamp = timer();
		for (unsigned int i = 0; i < bytesToSend; i++) {
			SendBufferItem *nextBufferItemToSend = &buffer[nextBufferItemToSendIndex];
			unsigned char packetData = nextBufferItemToSend->data;
			unsigned int packetSeq = nextBufferItemToSend->seq;
			Packet packet(packetData, packetSeq);
			if (sock.socketSend(packet.bytes(), Packet::SIZE) <= 0) {
				log_error("Failed to send packet (seq: " + toStr(nextBufferItemToSend->seq) + ")");
			}
			nextBufferItemToSend->timestamp = currentTimestamp;
			log_info("Sent packet (seq: " + toStr(packet.getSeq()) + ", data: " + toStr(packet.getData()) + ", timer: " + toStr(nextBufferItemToSend->timestamp) + ")");
			nextSentSeq++;
			nextBufferItemToSendIndex++;
		}

		/* Listen for ACK */
		log_debug("Listening for ACK...");
		unsigned char ackMessage[AckPacket::SIZE];
		int recvSize = sock.socketRecv(ackMessage, AckPacket::SIZE);
		if (recvSize > 0) {
			AckPacket ackPacket(ackMessage);
			if (ackPacket.isValid()) {
				if ((ackPacket.getNextSeq() - nextAckSeq) <= (nextSentSeq - nextAckSeq)) {
					log_info("Received ACK (nextSeq: " + toStr(ackPacket.getNextSeq()) + ", adv: " + toStr((unsigned int) ackPacket.getAdv()) + ")");
					nextAckSeq = ackPacket.getNextSeq();
					while (!buffer.empty() && buffer.front().seq != nextAckSeq) {
						buffer.pop_front();
						nextBufferItemToSendIndex--;
						messageBytesAcked++;
					}
					advBytes = (unsigned int) ackPacket.getAdv();
				} else {
					log_info("Rejected ACK outside window (nextSeq: " + toStr(ackPacket.getNextSeq()) + ")");
					log_debug("(nextSentSeq: " + toStr(nextSentSeq) + ", nextAckSeq: " + toStr(nextAckSeq) + ")");
				}
			} else {
				log_info("Rejected ACK with invalid checksum");
			}
		}

		// TODO: also resend if receiving duplicate ACKs for previous packet

		/* Update packet timeouts, resend timeout packets */
		currentTimestamp = timer();
		std::deque<SendBufferItem>::iterator nextBufferItemToCheck = buffer.begin();
		unsigned int packetsAwaitingAck = nextSentSeq - nextAckSeq;
		log_debug("Updating timeout... (timestamp: " + toStr(currentTimestamp) + ")");
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

		/* Handle advertised window size == 0 case */
		if (advBytes == 0) {
			advBytes = 1;
			log_info("Advertised window size is 0 - set to 1 to allow periodical probes");
		}
	}

	return messageBytesAcked;
}
