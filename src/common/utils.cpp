#include "utils.h"
#include <ctime>

/* Returns current timestamp in nanoseconds */
unsigned long timer() {
	struct timespec currentTime;
	if (clock_gettime(CLOCK_MONOTONIC, &currentTime)) {
		return 0;
	}
	return currentTime.tv_sec*1000000000 + currentTime.tv_nsec;
}
