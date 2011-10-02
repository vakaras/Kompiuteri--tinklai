#ifndef DICT_SOCKET_H
#define DICT_SOCKET_H 1


#define INVALID_SOCKET -1
#define SOCKET_ERROR -1


#include <cstring>
#include <string>
#include <exception>


namespace dict {
namespace socket {

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>


#define SOCKET int
#define MAX_CHAR 256
#define MAX_WAITING_CONNECTIONS 3
#define DEFAULT_PORT 1234


class BaseException: public std::exception {

private:

  char *msg;

public:

  BaseException() {
    this->msg = NULL;
    }
  
  BaseException(const char *msg) {
    int len = strlen(msg);
    this->msg = new char[len + 1];
    strcpy(this->msg, msg);
    }

  virtual const char *what() const throw() {
    if (this->msg) {
      return this->msg;
      }
    else {
      return "Socket exception happened.";
      }
    }
  
  };


class ServerSocket {

private:
public:

  SOCKET descriptor;
  std::string host_name;
  sockaddr_in address;
  hostent *host_entry;


  void init_address(int port);
  void init_socket();


  ServerSocket(int port=DEFAULT_PORT);
  ~ServerSocket();

  std::string get_host_name();
  std::string get_ip_address();

  };

};
};


#endif
