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
  int post = (argc > 3 ? atoi(argv[3]) : 25);

  MIMEHTML html("mail.html");
  MIMEMultipart msg(subject, recipient);
  msg.append(&html);
  for (int i = 7; i < argc; i++) {
    MIMEImage *img = new MIMEImage(
        argv[i], argv[i] + string("@astrauskas.lt"));
    msg.append(img);
    }

  send_message("vastrauskas@gmail.com", "localhost", 25,
      "vytautas@astrauskas.lt", "astrauskas.lt", msg);

  return 0;
  }
