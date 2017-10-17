#include <iostream>
#include "tx.h"
using namespace std;

int main() {
	
	cout << "SENDER" << endl;

	unsigned char data[] = "abcd";

	Connection conn("127.0.0.1", "12345");
	send_data(conn, data, 4);

	return 0;
}
