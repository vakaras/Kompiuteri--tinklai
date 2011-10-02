#include "server.h"


using namespace dict::server;


int main(int argc, const char *argv[]) {

  try {
    Server server(1029);
    server.start(stdout);
    }
  catch (std::exception &e) {
    printf("Error occured: %s\n", e.what());
    }

  return 0;
  }
