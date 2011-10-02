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

    std::shared_ptr< std::list<WordList::Word> > word_list =
      WordList(socket).get_words();

    printf("Found %d matches:\n", (int) word_list->size());
    for (std::list<WordList::Word>::iterator i = word_list->begin();
        i != word_list->end(); i++) {
      printf("\n  %s:\n", i->value.c_str());
      printf("%s\n", i->meaning.c_str());
      }

    }

  return 0;
  }
