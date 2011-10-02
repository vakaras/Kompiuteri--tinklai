#ifndef DICT_SERVER_H
#define DICT_SERVER_H 1

//#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <sys/time.h>
//#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>

#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define MAX_CHAR 256
#define SERVER_PORT 1234
#define MAX_WAITING_CONNECTIONS 3

#include <cstring>
#include <cstdio>

#endif
