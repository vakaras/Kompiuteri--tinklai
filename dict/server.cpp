#include "server.h"


using namespace dict::server;


int main(int argc, const char *argv[]) {

  try {
    Server server((argc > 1 ? argv[1] : "de.dict"), DEFAULT_PORT);
    server.start(stdout);
    }
  catch (std::exception &e) {
    printf("Error occured: %s\n", e.what());
    }

  return 0;
  }
