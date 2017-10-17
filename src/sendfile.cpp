#include <iostream>
#include "send_connection.h"
using namespace std;

int main() {
	
	cout << "SENDER" << endl;

	unsigned char data[1024] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer sed elit sed massa convallis faucibus. Curabitur elementum tristique est nec auctor. Mauris suscipit sodales interdum. Donec a pellentesque eros. Donec vel ex pulvinar, auctor arcu nec, pellentesque magna. Cras porta, metus ut egestas viverra, enim ligula tincidunt arcu, at mattis nunc metus vitae erat. Curabitur rhoncus purus eu elementum malesuada. Phasellus diam orci, sollicitudin sed facilisis nec, facilisis in urna. Curabitur imperdiet ex metus, quis cursus neque vestibulum et. Fusce euismod elementum ante, vitae massa nunc.";

	SendConnection conn("localhost", "12345");
	if (!conn.isValid()) {
		return 1;
	}

	conn.setSendWindowSize(8);
	conn.setAdvertisedWindowSize(8);

	conn.send_data(data, 600);
	cout << "Finished sending data." << endl;

	return 0;
}
