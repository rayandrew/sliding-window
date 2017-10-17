#ifndef SW_FRAME_H
#define SW_FRAME_H

class Frame {
public:
	Frame(unsigned char data, unsigned int seq);
	Frame(unsigned char *frame);

	bool isValid();
	unsigned char* bytes();
	unsigned char getData();
	unsigned int getSeq();

	static const unsigned int SIZE = 9;

private:
	static const unsigned char SOH = 0x1;
	static const unsigned char STX = 0x2;
	static const unsigned char ETX = 0x3;
	unsigned char frame[SIZE];
};

class AckFrame {
public:
	AckFrame(unsigned int nextSeq, unsigned char adv);
	AckFrame(unsigned char *frame);

	bool isValid();
	unsigned char* bytes();
	unsigned char getAdv();
	unsigned int getNextSeq();

	static const unsigned int SIZE = 7;

private:
	static const unsigned char ACK = 0x6;
	unsigned char frame[SIZE];
};

#endif
