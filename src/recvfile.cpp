#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include "recv_connection.h"
#include "utils.h"

using namespace std;

char *filename, *port;
unsigned int windowSize, bufferSize;
unsigned char *fileBuffer;

void printUsageAndExit(const char *exeName) {
	cerr << "Usage: " << exeName << " <filename> <windowsize> <buffersize> <port>" << endl;
	exit(1);
}

int main(int argc, char* argv[]) {
	
	cout << ":: RECEIVER ::" << endl;

	if (argc != 5) printUsageAndExit(argv[0]);
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
       
	port = argv[4];

	/** Create file for logging */
	std::ofstream fl;
	string logfile = "./logs/recvfile_" + toStr(timer()) + ".txt";
	fl.open(logfile.c_str());
	create_logger(fl, std::cout);

	/* Open file for writing */
	ofstream fout(filename, ifstream::binary);
	if (fout.is_open()) {

		/* Create a new connection, listening on all addresses */
		RecvConnection conn(NULL, port);
		if (!conn.isValid()) exit(1);
		//conn.setReceiveWindowSize(windowSize);
		conn.setReceiveTimeout(200000);

		/* Receive packets and write to file */
		fileBuffer = new unsigned char[bufferSize];
		unsigned long long receivedBytes = 0;
		while (!conn.eot()) {
			int connReadBytes = conn.recv_data(fileBuffer, bufferSize);
			if (connReadBytes < 0) {
				cerr << "Failed to read data from connection." << endl;
				exit(1);
			}
			receivedBytes += connReadBytes;

			fout.write((char *) fileBuffer, connReadBytes);
		}
		delete[] fileBuffer;
		fout.close();

	} else {
		cerr << "Unable to open file (" << filename << ")." << endl;
		exit(1);
	}

	cout << "Finished receiving file (" << filename << ")." << endl;

	/** close logfile **/
	fl.close();
	return 0;
}
