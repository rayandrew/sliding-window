#include "recv_connection.h"
#include "packet.h"
#include "utils.h"
#include <algorithm>

RecvConnection::RecvConnection(const char *host, const char *port) : sock(host, port, true) {
	receiveWindowSize = 256;
	recvTimeout = 2 * MICROSECONDS_IN_A_SECOND;

	nextValidatedSeq = 0;
	nextRecvSeq = 0;
}

int RecvConnection::recv_data(unsigned char *message, unsigned int messageSize) {
	sock.setRecvTimeout(recvTimeout);

	unsigned int messageBytesValidated = 0;
	while (messageBytesValidated < messageSize) {

		/* Listen for packets */
		log_debug("Listening for packets...");
		unsigned char packetBytes[Packet::SIZE];
		int recvSize = sock.socketRecvFrom(packetBytes, Packet::SIZE, (struct sockaddr *) &lastReceivedAddress, &lastReceivedAddressLength);
		if (recvSize > 0) {
			Packet packet(packetBytes);
			if (packet.isValid()) {
				if ((packet.getSeq() - nextValidatedSeq) < receiveWindowSize) {
					log_info("Received packet (seq: " + toStr(packet.getSeq()) + ", data: " + toStr(packet.getData()) + ")");

					/* Advance nextRecvSeq */
					if ((packet.getSeq() - nextValidatedSeq) >= (nextRecvSeq - nextValidatedSeq)) {
						nextRecvSeq = packet.getSeq() + 1;
					}

					/* Write validated packets to result */
					buffer[packet.getSeq()] = packet.getData();
					while (buffer.find(nextValidatedSeq) != buffer.end()) {
						message[messageBytesValidated] = buffer[nextValidatedSeq];
						buffer.erase(nextValidatedSeq);
						messageBytesValidated++;
						nextValidatedSeq++;
					}

					/* Reply with ACK */
					unsigned char adv = std::min((unsigned int) 255, receiveWindowSize - (nextRecvSeq - nextValidatedSeq));
					AckPacket ackPacket(nextValidatedSeq, adv);
					if (sock.socketSendTo(ackPacket.bytes(), AckPacket::SIZE, (struct sockaddr *) &lastReceivedAddress, lastReceivedAddressLength) <= 0) {
						log_error("Failed to send ACK (nextSeq: " + toStr(ackPacket.getNextSeq()) + ")");
					}
					log_info("Sent ACK (nextSeq: " + toStr(ackPacket.getNextSeq()) + ", adv: " + toStr((unsigned int) ackPacket.getAdv()) + ")");

				} else {
					log_info("Rejected packet outside window (seq: " + toStr(packet.getSeq()) + ")");
				}
			} else {
				log_info("Rejected packet with invalid checksum");
			}
		}
	}

	return messageBytesValidated;
}
