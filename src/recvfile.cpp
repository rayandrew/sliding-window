#include <iostream>
#include "recv_connection.h"
using namespace std;

int main() {
	
	cout << "RECEIVER" << endl;

	unsigned char data[4096];

	RecvConnection conn(NULL, "12345");
	if (!conn.isValid()) {
		return 1;
	}

	conn.setReceiveWindowSize(8);

	conn.recv_data(data, 600);
	cout << "Received data: " << data << endl;

	return 0;
}
