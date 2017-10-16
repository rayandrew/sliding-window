#include "connection.h"

Connection::Connection(int sock)
{
    socket = sock;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
}

Connection::Connection(int sock, int second, int microsecond)
{
    socket = sock;
    timeout.tv_sec = second;
    timeout.tv_usec = microsecond;
}

void Connection::setTimeout(int second, int microsecond)
{
    timeout.tv_sec = second;
    timeout.tv_usec = microsecond;
}

ssize_t Connection::receives(struct sockaddr_in *connection, char *buf, int *length)
{
    fd_set socks;
    FD_ZERO(&socks);
    FD_SET(socket, &socks);

    int fromlen;

    int checkTimeout = select(socket + 1, &socks, NULL, NULL, &timeout);
    return (checkTimeout == 0 || checkTimeout == -1) ? 0 : recvfrom(socket, buf, *length, 0, (struct sockaddr *)connection, (socklen_t *)&fromlen);
}

ssize_t Connection::sends(struct sockaddr_in *connection, const char *buf, int *length)
{
    fd_set socks;
    FD_ZERO(&socks);
    FD_SET(socket, &socks);

    int fromlen;

    int checkTimeout = select(socket + 1, &socks, NULL, NULL, &timeout);
    return (checkTimeout == 0 || checkTimeout == -1) ? 0 : sendto(socket, buf, *length, 0, (struct sockaddr *)connection, (socklen_t)fromlen);
}