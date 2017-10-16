#ifndef SW_FRAME_H
#define SW_FRAME_H

class Frame {
public:
	Frame(unsigned char data, unsigned int seq);
	Frame(unsigned char *frame);

	bool isValid();

private:
	const unsigned char SOH = 0x1;
	const unsigned char STX = 0x2;
	const unsigned char ETX = 0x3;
	unsigned char frame[9];
};

class AckFrame {
public:
	AckFrame::AckFrame(unsigned char adv, unsigned int nextSeq);
	AckFrame(unsigned char *frame);

	bool isValid();

private:
	const unsigned char ACK = 0x6;
	unsigned char frame[7];
};

#endif
