#ifndef SW_RX_H
#define SW_RX_H

#include "common/connection.h"

int recv_data(Connection &conn, unsigned char *msg, unsigned int size);

#endif
