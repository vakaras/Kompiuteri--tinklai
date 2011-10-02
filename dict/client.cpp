#include "client.h"


using namespace dict::socket;
using namespace dict::package;


int main(int argc, char *argv[]) {

  const char *server = (argc > 1 ? argv[1] : "v-skreitinukas");
  int port = (argc > 2 ? atol(argv[2]) : DEFAULT_PORT);

  printf("Connecting: %s:%d\n", server, port);

  std::shared_ptr<ClientSocket> socket = std::shared_ptr<ClientSocket>(
      new ClientSocket(server, port));

  char buffer[MAX_CHAR];

  while (true) {

    printf("Enter word: ");
    scanf("%s", buffer);
    printf("Request (length=%d): \"%s\".\n", (int) strlen(buffer), buffer);

    Word word = Word(std::string(buffer));
    word.send(socket);

    }

  return 0;
  }
