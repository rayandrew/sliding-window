#include "recv_buffer.h"

unsigned int RecvBuffer::seqToIndex(unsigned int seq) {
	return seq % (capacity+1);
}


/* Capacity must be a factor of 2^32 */
RecvBuffer::RecvBuffer(unsigned int capacity) {
	this->capacity = capacity;
	buffer = new unsigned int[capacity+1];
	head = 0;
	tail = 0;
}

RecvBuffer::~RecvBuffer() {
	delete buffer[];
}

/**
 * Put data at specified sequence number in the buffer.
 * Does not have to be contiguous, as long as seq is in the window.
 */
void RecvBuffer::put(unsigned char data, unsigned int seq) {
	unsigned char val = data[head % (this->capacity + 1)];
	head++;
	return val;
}

/**
 * Get data at specified sequence number in the buffer.
 * Does not have to be contiguous, as long as seq is in the window.
 */
unsigned char RecvBuffer::get(unsigned int seq) {
	unsigned char val = data[head % (this->capacity + 1)];
	head++;
	return val;
}

unsigned int RecvBuffer::size() {
	return tail - head;
}

bool RecvBuffer::isFull() {
	return (tail - head) == this->capacity;
}

bool RecvBuffer::isEmpty() {
	return (tail - head) == 0;
}

bool RecvBuffer::isSeqInWindow(unsigned int seq) {
	return 
}
