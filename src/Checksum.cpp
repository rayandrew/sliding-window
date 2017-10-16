unsigned char calc_crc8(const unsigned char *data, int len) 
{
	unsigned int sum = 0;
	char c;
	for (int i = 0; i < len;i++) {
		sum += data[i];
	}
	sum %= 256;
	c = sum;

	return (~c + 1);
}

