#include "sender.h"


using namespace dict::socket;
using namespace std;


int get_code(const char *buffer) {
  return 
    (buffer[0] - '0') * 100 +
    (buffer[1] - '0') * 10 +
    (buffer[2] - '0');
  }


void smtp::sender::send_message(
    string recipient, string server, int port,
    string author, string sender_server, 
    smtp::mime::MIMEMultipart message) {

  ClientSocket socket(server.c_str(), port);

  char buffer[MAX_CHAR];
  socket.read_block((void *) buffer, 1, MAX_CHAR);
  printf("Received: %s\n", buffer);
  if (get_code(buffer) != 220) {
    throw BaseException(
        (string("Wrong return code. Expected: 220. Message: ") +
        string(buffer)).c_str());
    }

  // Pasisveikinimas.
  string hello = "ehlo " + sender_server;
  socket.write(hello.c_str(), 1, hello.length());
  while (true) {
    socket.read_block((void *) buffer, 1, MAX_CHAR);
    printf("Received: %s\n", buffer);
    if (get_code(buffer) != 250) {
      throw BaseException(
          (string("Wrong return code. Expected: 250. Message: ") +
          string(buffer)).c_str());
      }
    if (buffer[3] == ' ') {
      break;
      }
    }
  
  }
