#ifndef SW_RECV_BUFFER_H
#define SW_RECV_BUFFER_H

class RecvBuffer {
public:
	RecvBuffer(unsigned int capacity);
	~RecvBuffer();

	void put(unsigned char data, unsigned int seq);
	unsigned char RecvBuffer::popFront();
	unsigned char RecvBuffer::get(unsigned int seq);
	unsigned int RecvBuffer::size();
	bool RecvBuffer::isFull();
	bool RecvBuffer::isEmpty();
	bool RecvBuffer::isSeqInWindow(unsigned int seq);

private:

	unsigned int seqToIndex(unsigned int seq);

	unsigned int lastReceived;
	unsigned int lastAck;
	unsigned int windowSize;
	unsigned char *buffer;
	bool *received;
};

#endif
