#ifndef FILE_H
#define FILE_H

#include <fstream>

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

unsigned char* readFile(const char* filename, int nbyte)
{
	char* fileContent;
	ifstream myfile(filename, ios::in | ios::binary | ios::ate);
	if (myfile.is_open()) {
		fileContent = new char[nbyte];		
		myfile.seekg(0, ios::beg);
		myfile.read(fileContent, nbyte);
		myfile.close();
	}
	return (unsigned char*)fileContent;
	//Can cause memory leak
}

#endif
