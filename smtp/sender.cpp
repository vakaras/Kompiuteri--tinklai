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
  while (socket.read((void *) buffer, 1, MAX_CHAR)) {
    printf("Received: %s\n", buffer);
    }
  
  }
