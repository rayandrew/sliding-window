#ifndef SW_UTILS_H
#define SW_UTILS_H

#define MICROSECONDS_IN_A_SECOND 1000000
#define NANOSECONDS_IN_A_SECOND 1000000000
#define MICROSECONDS_IN_A_NANOSECOND 1000

#include <iostream>
#include <sstream>
#include <fstream>

unsigned long timer();

void create_logger(std::ostream& out1, std::ostream& out2);
void log_info(std::string message);
void log_error(std::string message);
void log_debug(std::string message);

unsigned char compressAdv(unsigned int adv);
unsigned int extractAdv(unsigned char input);

template <typename T> std::string toStr(const T& n) {
    std::ostringstream stm;
    stm << n;
    return stm.str();
}

#endif
