#ifndef SW_TX_H
#define SW_TX_H

#include "common/connection.h"

int send_data(Connection &conn, unsigned char *msg, unsigned int size);

#endif
