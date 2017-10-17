#ifndef CHECKSUM_H
#define CHECKSUM_H

/* CRC-8 */
unsigned char checksum(const unsigned char *data, unsigned int len) {
	const unsigned char polynomial = 0x1D;
	unsigned char res = 0;

	for (unsigned int i = 0; i < len; i++) {
		res ^= data[i];
		for (unsigned int j = 0; j < 8; j++) {
			if ((res & 0x80) != 0) {
				res = (unsigned char) ((res << 1) ^ polynomial);
			} else {
				res <<= 1;
			}
		}
	}

	return res;
}

#endif
