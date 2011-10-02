#ifndef DICT_SOCKET_H
#define DICT_SOCKET_H 1


#define INVALID_SOCKET -1
#define SOCKET_ERROR -1


#include <cstring>
#include <string>
#include <exception>
#include <memory>


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


class Socket {

protected:

  SOCKET descriptor;
  sockaddr_in address;
  hostent *host_entry;

  void set_reuse_option();

public:

  ~Socket();

  std::string get_ip_address();
  SOCKET get_descriptor();

  };


class ClientSocket: public Socket {

private:

  FILE *read_connection;
  FILE *write_connection;

public:

  ClientSocket(SOCKET server_socket_descriptor);
  ~ClientSocket();

  size_t read(void *buffer, size_t size, size_t count);
  int write(const void *buffer, size_t size, size_t count);
  void flush();

  };


class ServerSocket: public Socket {

private:

  std::string host_name;

  void init_address(int port);
  void init_socket();

public:

  ServerSocket(int port=DEFAULT_PORT);

  std::string get_host_name();
  std::shared_ptr<ClientSocket> accept_client();

  };

};
};

#endif
