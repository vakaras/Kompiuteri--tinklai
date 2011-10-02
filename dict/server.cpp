#include "server.h"


int main(int argc, const char *argv[]) {

  SOCKET socket_descriptor = -1;
  char server_host_name[MAX_CHAR] = "";
  sockaddr_in server_address;
  hostent *server_host_entry = NULL;

  printf("Server: Starting.\n");

  if (INVALID_SOCKET == gethostname(
        server_host_name, sizeof(server_host_name))) {
    return INVALID_SOCKET;
    }

  if (NULL == (server_host_entry = gethostbyname(server_host_name))) {
    return INVALID_SOCKET;
    }

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(SERVER_PORT);
  server_address.sin_addr = *((in_addr *)server_host_entry->h_addr);
  memset(&(server_address.sin_zero), 0, 8);

  printf("Server: Address constructed.\n");

  if (INVALID_SOCKET == (
        socket_descriptor = socket(AF_INET, SOCK_STREAM, 0))) {
    return INVALID_SOCKET;
    }

  printf("Server: Socket created.\n");

  const int yes = 1;
  if (SOCKET_ERROR == setsockopt(
        socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
    return INVALID_SOCKET;
    }

  printf("Server: Socket reuse address flag set.\n");

  if (SOCKET_ERROR == bind(
        socket_descriptor,
        (sockaddr *)&server_address,
        sizeof(server_address))) {
    close(socket_descriptor);
    return INVALID_SOCKET;
    }

  printf("Server: Socket binded.\n");

  if (SOCKET_ERROR == listen(socket_descriptor, MAX_WAITING_CONNECTIONS)) {
    close(socket_descriptor);
    return INVALID_SOCKET;
    }

  printf(
      "Server: Started successfully on host \'%s\' – (%s).\n",
      server_host_name,
      inet_ntoa(*((in_addr *)server_host_entry->h_addr))
      );

  while (true) {
    }

  return 0;
  }
