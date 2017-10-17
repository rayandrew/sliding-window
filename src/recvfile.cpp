#include <iostream>
#include "rx.h"

using namespace std;

int main() {
	
	cout << "RECEIVER" << endl;

	unsigned char data[4096];

	Connection conn(NULL, "12345");
	if (!conn.isValid()) {
		return 1;
	}

	recv_data(conn, data, 4);

	cout << "Received data: " << data << endl;

	return 0;
}
