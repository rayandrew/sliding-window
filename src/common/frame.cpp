#include "frame.h"
//#include "checksum.h" TODO: implement checksum
#include <cstring>

Frame::Frame(unsigned char data, unsigned int seq) {
	this->frame[0] = SOH;
	memcpy(this->frame+1, &seq, sizeof(seq));
	this->frame[5] = STX;
	this->frame[6] = data;
	this->frame[7] = ETX;
	//this->frame[8] = checksum(this->frame, SIZE-1);
}

Frame::Frame(unsigned char *frame) {
	memcpy(this->frame, frame, SIZE);
}

bool Frame::isValid() {
	return true; // TODO
	//return checksum(this->frame, 8) == this->frame[8];
}

unsigned char* Frame::bytes() {
	return this->frame;
}

unsigned char Frame::getData() {
	return this->frame[6];
}

unsigned int Frame::getSeq() {
	unsigned int seq;
	memcpy(&seq, this->frame+1, sizeof(seq));
	return seq;
}

AckFrame::AckFrame(unsigned char adv, unsigned int nextSeq) {
	this->frame[0] = ACK;
	memcpy(this->frame+1, &nextSeq, sizeof(seq));
	this->frame[5] = adv;
	//this->frame[6] = checksum(this->frame, SIZE-1);
}

AckFrame::AckFrame(unsigned char *frame) {
	memcpy(this->frame, frame, SIZE);
}

bool AckFrame::isValid() {
	return true; // TODO
	//return checksum(this->frame, SIZE-1) == this->frame[SIZE-1];
}

unsigned char* AckFrame::bytes() {
	return this->frame;
}

unsigned char AckFrame::getAdv() {
	return this->frame[5];
}

unsigned int AckFrame::getNextSeq() {
	unsigned int nextSeq;
	memcpy(&nextSeq, this->frame+1, sizeof(nextSeq));
	return nextSeq;
}
