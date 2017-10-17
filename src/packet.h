#ifndef SW_PACKET_H
#define SW_PACKET_H

class Packet {
public:
	Packet(unsigned char data, unsigned int seq);
	Packet(unsigned char *packet);

	bool isValid();
	unsigned char* bytes();
	unsigned char getData();
	unsigned int getSeq();

	static const unsigned int SIZE = 9;

private:
	static const unsigned char SOH = 0x1;
	static const unsigned char STX = 0x2;
	static const unsigned char ETX = 0x3;
	unsigned char packet[SIZE];
};

class AckPacket {
public:
	AckPacket(unsigned int nextSeq, unsigned char adv);
	AckPacket(unsigned char *packet);

	bool isValid();
	unsigned char* bytes();
	unsigned char getAdv();
	unsigned int getNextSeq();

	static const unsigned int SIZE = 7;

private:
	static const unsigned char ACK = 0x6;
	unsigned char packet[SIZE];
};

#endif
