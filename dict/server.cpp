#include "server.h"


int main(int argc, const char *argv[]) {

  try {
    dict::socket::ServerSocket server_socket(1029);
    printf(
        "Server: Started successfully on host \'%s\' – (%s).\n",
        server_socket.get_host_name().c_str(),
        server_socket.get_ip_address().c_str()
        );
    while (true) {
      }
    }
  catch (std::exception &e) {
    printf("Error occured: %s\n", e.what());
    }

  return 0;
  }
