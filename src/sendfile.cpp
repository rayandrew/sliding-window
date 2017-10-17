#include <iostream>
#include "send_connection.h"
using namespace std;

int main() {
	
	cout << "SENDER" << endl;

	unsigned char data[] = "abcd";

	SendConnection conn("127.0.0.1", "12345");
	if (!conn.isValid()) {
		return 1;
	}

	conn.send_data(data, 4);
	cout << "Finished sending data." << endl;

	return 0;
}
