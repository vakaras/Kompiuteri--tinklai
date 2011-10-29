#ifndef SMTP_SENDER_H
#define SMTP_SENDER_H


#include <cstdio>
#include <cstdlib>
#include <string>
#include "socket.h"
#include "mime.h"


namespace smtp {
namespace sender {

void send_message(
    std::string recipient, std::string server, int port,
    std::string author, std::string sender_server, 
    smtp::mime::MIMEMultipart message);

};
};



#endif
