#ifndef CHECKSUM_H
#define CHECKSUM_H

unsigned char checksum(const unsigned char *data, unsigned int len) 
{
	unsigned int sum = 0;
	char c;
	for (unsigned int i = 0; i < len;i++) {
		sum += data[i];
	}
	sum %= 256;
	c = sum;

	return (~c + 1);
}

#endif
