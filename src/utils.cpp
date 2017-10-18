#include "utils.h"
#include <ctime>

#include "logger.h"

Logger *log;

/* Returns current timestamp in nanoseconds */
unsigned long timer() {
	struct timespec currentTime;
	if (clock_gettime(CLOCK_MONOTONIC, &currentTime)) {
		return 0;
	}
	return currentTime.tv_sec*NANOSECONDS_IN_A_SECOND + currentTime.tv_nsec;
}

void create_logger(std::ostream& out1, std::ostream& out2) {
	log = new Logger(out1, out2);
}

void log_info(std::string message) {
	*log << "[ INFO ] " << message << std::endl;
}

void log_error(std::string message) {
	*log << "[ ERROR ] " << message << std::endl;
}

void log_debug(std::string message) {
	*log << "[ DEBUG ] " << message << std::endl;
}
