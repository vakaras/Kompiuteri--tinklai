#include "sender.h"


using namespace dict::socket;
using namespace std;


void smtp::sender::send_message(
    string recipient, string server, int port,
    string author, string sender_server, 
    smtp::mime::MIMEMultipart message) {

  ClientSocket socket(server.c_str(), port);

  char buffer[MAX_CHAR];

  // Pasisveikinimas.
  string hello = "ehlo " + sender_server;
  socket.write(hello.c_str(), 1, hello.length());
  while (true) {
    socket.read_block((void *) buffer, 1, MAX_CHAR);
    printf("Received: %s\n", buffer);
    if (!(buffer[0] == '2' && buffer[1] == '5' && buffer[2] == '0')) {
      throw BaseException(
          (string("Wrong return code. Expected: 250. Message: ") +
          string(buffer)).c_str());
      }
    if (buffer[3] == ' ') {
      break;
      }
    }
  
  }
