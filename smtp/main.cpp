#include <cstring>
#include "encoders.h"
#include "utils.h"
#include "mime.h"
#include "sender.h"


using namespace smtp::encoders;
using namespace smtp::utils;
using namespace smtp::mime;
using namespace smtp::sender;


#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>


int main(int argc, const char *argv[]) {


  MIMEHTML html("mail.html");
  MIMEImage img("joana.jpeg", "joana@astrauskas.lt");
  MIMEMultipart msg(
      "Testas 01: Lietuviškos raidės.", "vastrauskas@gmail.com");
  msg.append(&html);
  msg.append(&img);

  //msg.write(stdout);

  send_message("vastrauskas@gmail.com", "localhost", 25,
      "vytautas@astrauskas.lt", "astrauskas.lt", msg);

  return 0;
  }
