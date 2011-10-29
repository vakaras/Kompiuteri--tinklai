#include "sender.h"


using namespace dict::socket;
using namespace std;


int get_code(const char *buffer) {
  return 
    (buffer[0] - '0') * 100 +
    (buffer[1] - '0') * 10 +
    (buffer[2] - '0');
  }


bool get_line(FILE *fin, int *code, char *buffer) {
  fscanf(fin, "%d", code);
  bool end = fgetc(fin) == ' ';
  for (int i = 0; char c = fgetc(fin); i++) {
    if (c == '\n') {
      buffer[i++] = 0;
      break;
      }
    else {
      buffer[i++] = c;
      }
    }
  return end;
  }


void smtp::sender::send_message(
    string recipient, string server, int port,
    string author, string sender_server, 
    smtp::mime::MIMEMultipart message) {

  ClientSocket socket(server.c_str(), port);

  FILE *fin = socket.get_read_connection();
  FILE *fout = socket.get_write_connection();

  char buffer[MAX_CHAR];
  char msg[MAX_CHAR];
  int code;

  get_line(fin, &code, msg);
  //fscanf(fin, "%d %s\n", &code, msg);
  printf("Received (%d): %s\n", code, msg);
  if (code != 220) {
    throw BaseException("Wrong return code. Expected: 220.");
    }


  //socket.read_block((void *) buffer, 1, MAX_CHAR);
  //printf("Received: %s\n", buffer);
  //if (get_code(buffer) != 220) {
    //throw BaseException(
        //(string("Wrong return code. Expected: 220. Message: ") +
        //string(buffer)).c_str());
    //}

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
