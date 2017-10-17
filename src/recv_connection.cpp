#include "recv_connection.h"
#include "packet.h"
#include "utils.h"
#include <algorithm>

RecvConnection::RecvConnection(const char *host, const char *port) : sock(host, port, true) {
	receiveWindowSize = 256;
	recvTimeout = 2 * MICROSECONDS_IN_A_SECOND;

	nextValidatedSeq = 0;
	nextRecvSeq = 0;

	lastReceivedAddressLength = sizeof(lastReceivedAddress);

	checkEot = false;
	reachedEot = false;
}

int RecvConnection::recv_data(unsigned char *message, unsigned int messageSize) {
	log_info("Receiving message (size: " + toStr(messageSize) + " bytes)...");
	sock.setRecvTimeout(recvTimeout);

	unsigned int messageBytesValidated = 0;
	while (messageBytesValidated < messageSize) {

		/* End receive on EOT */
		if (checkEot && nextValidatedSeq == eotSeq) {
			reachedEot = true;
		}
		if (reachedEot) {
			log_debug("Ending receive due to EOT");
			checkEot = false;
			return messageBytesValidated;
		}

		/* Listen for packets */
		log_debug("Listening for packets...");
		unsigned char packetBytes[Packet::SIZE];
		int recvSize = sock.socketRecvFrom(packetBytes, Packet::SIZE, (struct sockaddr *) &lastReceivedAddress, &lastReceivedAddressLength);
		if (recvSize > 0) {
			Packet packet(packetBytes);
			if (packet.isValid()) {

				/* Detect if the received packet is an EOT packet; reply with EOT_ACK */
				if (packet.isEndOfTransmission()) {
					log_info("Received EOT (eotSeq: " + toStr(packet.getSeq()) + ")");
					checkEot = true;
					eotSeq = packet.getSeq();

					AckPacket ackPacket(packet.getSeq(), 0);
					ackPacket.setEndOfTransmission();
					if (sock.socketSendTo(ackPacket.bytes(), AckPacket::SIZE, (struct sockaddr *) &lastReceivedAddress, lastReceivedAddressLength) <= 0) {
						log_error("Failed to send EOT_ACK (eotSeq: " + toStr(ackPacket.getNextSeq()) + ")");
					}
					
				} else {

					if ((packet.getSeq() - nextValidatedSeq) < receiveWindowSize) {
						log_info("Received packet (seq: " + toStr(packet.getSeq()) + ", data: " + toStr(packet.getData()) + ")");
						log_debug("Packet received from: " + toStr(inet_ntop(lastReceivedAddress.ss_family, Socket::getInAddress((struct sockaddr *) &lastReceivedAddress), lastReceivedAddressString, sizeof(lastReceivedAddressString))) );

						/* Advance nextRecvSeq */
						if ((packet.getSeq() - nextValidatedSeq) >= (nextRecvSeq - nextValidatedSeq)) {
							nextRecvSeq = packet.getSeq() + 1;
						}

						/* Write validated packets to result */
						buffer[packet.getSeq()] = packet.getData();
						while (buffer.find(nextValidatedSeq) != buffer.end()) {
							if (checkEot && nextValidatedSeq == eotSeq) { // Check in case EOT is arrives before outstanding packets
								reachedEot = true;
							}
							if (!checkEot || !reachedEot) {
								message[messageBytesValidated] = buffer[nextValidatedSeq];
								messageBytesValidated++;
							}
							buffer.erase(nextValidatedSeq);
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

				}
			} else {
				log_info("Rejected packet with invalid checksum");
			}
		}
	}

	return messageBytesValidated;
}
