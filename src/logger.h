#ifndef SW_LOGGER_H
#define SW_LOGGER_H

#include <iostream>
#include <fstream>

struct Logger
{
    Logger(std::ostream& out1, std::ostream& out2) : out1_(out1), out2_(out2) {}
    std::ostream& out1_;
    std::ostream& out2_;
};

template <typename T>
Logger& operator<<(Logger& h, T const& t)
{
   h.out1_ << t;
   h.out2_ << t;
   return h;
}

Logger& operator<<(Logger& h, std::ostream&(*f)(std::ostream&))
{
   h.out1_ << f;
   h.out2_ << f;
   return h;
}

#endif