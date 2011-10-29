#include <cstring>
#include "encoders.h"
#include "utils.h"
#include "mime.h"
#include "sender.h"


using namespace smtp::encoders;
using namespace smtp::utils;
using namespace smtp::mime;
using namespace smtp::sender;
using namespace std;


int main(int argc, const char *argv[]) {

  const char
    *recipient      = (argc > 1 ? argv[1] : "vastrauskas@gmail.com"),
    *server         = (argc > 2 ? argv[2] : "localhost"),
    *author         = (argc > 4 ? argv[4] : "vytautas@astrauskas.lt"),
    *sender_server  = (argc > 5 ? argv[5] : "astrauskas.lt"),
    *html_file      = (argc > 6 ? argv[6] : "mail.html"),
    *subject        = (argc > 7 ? argv[7] : "Testas 01.");
  int port = (argc > 3 ? atoi(argv[3]) : 25);

  printf("Argumentų kiekis:   %d\n", argc);
  printf("Gavėjas:            %s\n", recipient);
  printf("Serveris:           %s\n", server);
  printf("Prievadas:          %d\n", port);
  printf("Siuntėjas:          %s\n", author);
  printf("Siunėjo serveris:   %s\n", sender_server);
  printf("HTML failas:        %s\n", html_file);
  printf("Tema:               %s\n", subject);

  MIMEHTML html(html_file);
  MIMEMultipart msg(subject, recipient);
  msg.append(&html);
  for (int i = 8; i < argc; i++) {
    MIMEImage *img = new MIMEImage(
        argv[i], argv[i] + string("@astrauskas.lt"));
    msg.append(img);
    }

  send_message(recipient, server, port, author, sender_server, msg);


  return 0;
  }
