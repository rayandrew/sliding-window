#ifndef SW_TX_H
#define SW_TX_H

#include "common/connection.h"

int send_data(Connection &conn, unsigned char *message, unsigned int messageSize);

#endif
