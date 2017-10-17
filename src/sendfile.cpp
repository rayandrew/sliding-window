#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include "send_connection.h"
using namespace std;

unsigned char *fileBuffer;
char *filename, *host, *port;
unsigned int windowSize, bufferSize;

void printUsageAndExit(const char *exeName) {
	cerr << "Usage: " << exeName << " <filename> <windowsize> <buffersize> <destination_ip> <destination_port>" << endl;
	exit(1);
}

int main(int argc, char* argv[]) {
	
	cout << ":: SENDER ::" << endl;

	if (argc != 6) printUsageAndExit(argv[0]);
	filename = argv[1];

	std::istringstream issWindowSize(argv[2]);
	if (!(issWindowSize >> windowSize)) {
		cerr << "Window size must be a number." << endl;
		printUsageAndExit(argv[0]);
	}

	std::istringstream issBufferSize(argv[3]);
	if (!(issBufferSize >> bufferSize)) {
		cerr << "Buffer size must be a number." << endl;
		printUsageAndExit(argv[0]);
	}
       
	host = argv[4];
	port = argv[5];

	/* Create a new connection */
	SendConnection conn(host, port);
	if (!conn.isValid()) exit(1);
	conn.setSendWindowSize(windowSize);

	/* Open file for reading */
	ifstream fin(filename, ifstream::binary);
	if (fin.is_open()) {
		cout << "File " << filename << " opened." << endl;

		/* Read file segments and send */
		fileBuffer = new unsigned char[bufferSize];
		while (true) {
			fin.read((char*) fileBuffer, bufferSize);
			size_t bytesRead = fin.gcount();
			if (!bytesRead) break;
			conn.send_data(fileBuffer, bytesRead);
		}
		delete[] fileBuffer;
		fin.close();

		/* Send end-of-transmission packet */
		conn.sendEndOfTransmission();

	} else {
		cerr << "Unable to open file (" << filename << ")." << endl;
		exit(1);
	}

	cout << "Finished sending file (" << filename << ")." << endl;
	return 0;
}
