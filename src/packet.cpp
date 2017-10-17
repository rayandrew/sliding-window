#include "packet.h"
#include "checksum.h"
#include <cstring>

Packet::Packet(unsigned char data, unsigned int seq) {
	this->packet[0] = SOH;
	memcpy(this->packet+1, &seq, sizeof(seq));
	this->packet[5] = STX;
	this->packet[6] = data;
	this->packet[7] = ETX;
	this->packet[8] = checksum(this->packet, SIZE-1);
}

Packet::Packet(unsigned char *packet) {
	memcpy(this->packet, packet, SIZE);
}

bool Packet::isValid() {
	return this->packet[0] == SOH
		&& checksum(this->packet, 8) == this->packet[8]
		&& this->packet[5] == STX
		&& this->packet[7] == ETX;
}

unsigned char* Packet::bytes() {
	return this->packet;
}

unsigned char Packet::getData() {
	return this->packet[6];
}

unsigned int Packet::getSeq() {
	unsigned int seq;
	memcpy(&seq, this->packet+1, sizeof(seq));
	return seq;
}

AckPacket::AckPacket(unsigned int nextSeq, unsigned char adv) {
	this->packet[0] = ACK;
	memcpy(this->packet+1, &nextSeq, sizeof(nextSeq));
	this->packet[5] = adv;
	this->packet[6] = checksum(this->packet, SIZE-1);
}

AckPacket::AckPacket(unsigned char *packet) {
	memcpy(this->packet, packet, SIZE);
}

bool AckPacket::isValid() {
	return this->packet[0] == ACK
		&& checksum(this->packet, SIZE-1) == this->packet[SIZE-1];
}

unsigned char* AckPacket::bytes() {
	return this->packet;
}

unsigned char AckPacket::getAdv() {
	return this->packet[5];
}

unsigned int AckPacket::getNextSeq() {
	unsigned int nextSeq;
	memcpy(&nextSeq, this->packet+1, sizeof(nextSeq));
	return nextSeq;
}
