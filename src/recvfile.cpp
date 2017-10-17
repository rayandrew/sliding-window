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

	conn.recv_data(data, 4);
	cout << "Received data: " << data << endl;

	return 0;
}
