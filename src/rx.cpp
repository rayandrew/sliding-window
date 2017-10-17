#include "rx.h"
#include "common/frame.h"
#include "common/utils.h"
#include <algorithm>
#include <map>

unsigned int receiveWindowSize = 256;
unsigned long recvTimeout = 2 * MICROSECONDS_IN_A_SECOND;

unsigned int nextValidatedSeq = 0;
unsigned int nextRecvSeq = 0;

std::map<unsigned int, unsigned char> buffer;

void init_rx() { // TODO: config rx using this function
	receiveWindowSize = 256;
}

int send_data(Connection &conn, unsigned char *message, unsigned int messageSize) {
	conn.setRecvTimeout(recvTimeout);

	unsigned int messageBytesValidated = 0;
	while (messageBytesValidated < messageSize) {

		/* Listen for frames */
		log_debug("Listening for frames...");
		unsigned char frameBytes[Frame::SIZE];
		int recvSize = conn.rx(frameBytes, Frame::SIZE);
		if (recvSize > 0) {
			Frame frame(frameBytes);
			if (frame.isValid()) {
				if ((frame.getSeq() - nextValidatedSeq) < receiveWindowSize) {
					log_info("Received frame (seq: " + toStr(frame.getSeq()) + ", data: " + toStr(frame.getData()) + ")");

					/* Advance nextRecvSeq */
					if ((frame.getSeq() - nextValidatedSeq) >= (nextRecvSeq - nextValidatedSeq)) {
						nextRecvSeq = frame.getSeq() + 1;
					}

					/* Write validated packets to result */
					buffer[frame.getSeq()] = frame.getData();
					while (buffer.find(nextValidatedSeq) != buffer.end()) {
						message[messageBytesValidated] = buffer[nextValidatedSeq];
						buffer.erase(nextValidatedSeq);
						messageBytesValidated++;
						nextValidatedSeq++;
					}

					/* Reply with ACK */
					AckFrame ackFrame(nextValidatedSeq, receiveWindowSize - (nextRecvSeq - nextValidatedSeq));
					if (conn.tx(ackFrame.bytes(), AckFrame::SIZE) <= 0) {
						log_error("Failed to send ACK (nextSeq: " + toStr(ackFrame.getNextSeq()) + ")");
					}

				} else {
					log_info("Rejected frame outside window (seq: " + toStr(frame.getSeq()) + ")");
				}
			} else {
				log_info("Rejected frame with invalid checksum");
			}
		}
	}

	return messageBytesValidated;
}
