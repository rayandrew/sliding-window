#include "frame.h"
//#include "checksum.h" TODO: implement checksum
#include <cstring>

Frame::Frame(unsigned char data, unsigned int seq) {
	this->frame[0] = SOH;
	memcpy(this->frame+1, &seq, 4);
	this->frame[5] = STX;
	this->frame[6] = data;
	this->frame[7] = ETX;
	//this->frame[8] = checksum(this->frame, 8);
}

Frame::Frame(unsigned char *frame) {
	memcpy(this->frame, frame, 9);
}

bool Frame::isValid() {
	return true; // TODO
	//return checksum(this->frame, 8) == this->frame[8];
}

AckFrame::AckFrame(unsigned char adv, unsigned int nextSeq) {
	this->frame[0] = ACK;
	memcpy(this->frame+1, &nextSeq, 4);
	this->frame[5] = adv;
	//this->frame[6] = checksum(this->frame, 6);
}

AckFrame::AckFrame(unsigned char *frame) {
	memcpy(this->frame, frame, 7);
}

bool AckFrame::isValid() {
	return true; // TODO
	//return checksum(this->frame, 6) == this->frame[6];
}
