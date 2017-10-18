#include "utils.h"
#include <ctime>

/* Returns current timestamp in nanoseconds */
unsigned long timer() {
	struct timespec currentTime;
	if (clock_gettime(CLOCK_MONOTONIC, &currentTime)) {
		return 0;
	}
	return currentTime.tv_sec*NANOSECONDS_IN_A_SECOND + currentTime.tv_nsec;
}

void log_info(std::string message) {
	// std::cout << "[ INFO ] " << message << std::endl;
}

void log_error(std::string message) {
	std::cout << "[ ERROR ] " << message << std::endl;
}

void log_debug(std::string message) {
	// std::cout << "[ DEBUG ] " << message << std::endl;
}

unsigned char compressAdv(unsigned int adv) {
	return std::min((unsigned int) 255, adv/8);
}

unsigned int extractAdv(unsigned char input) {
	return ((unsigned int) input) * 8;
}