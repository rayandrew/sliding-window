#include "connection.h"

Connection::Connection(int sock, struct sockaddr_in *conn) {
    socket = sock;
    connection = conn;
}

Connection::Connection(int sock, struct sockaddr_in *conn, int second, int microsecond) {
    socket = sock;
    connection = conn;
    timeout.tv_sec = second;
    timeout.tv_usec = microsecond;
}

void Connection::setTimeout(int second, int microsecond) {
    timeout.tv_sec = second;
    timeout.tv_usec = microsecond;
}

int Connection::recvfrom(char *buf, int *length)
{
    fd_set socks;
    FD_ZERO(&socks);
    FD_SET(sock, &socks);
    return (select(socket + 1, &socks, NULL, NULL, &timeout) && recvfrom(socket, buf, *length, 0, (struct sockaddr *)connection, length);
}

int Connection::sendto() {
    
}