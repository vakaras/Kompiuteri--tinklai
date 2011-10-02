#ifndef DICT_SERVER_H
#define DICT_SERVER_H 1

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/time.h>
//#include <sys/wait.h>
//#include <unistd.h>
//#include <netdb.h>
#include <sys/select.h>

#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define MAX_CHAR 256
#define SERVER_PORT 1234
#define MAX_WAITING_CONNECTIONS 3

#include <cstring>
#include <cstdio>
#include <exception>
#include <list>

#include "socket.h"


namespace dict {
namespace server {


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
      return "Server exception happened.";
      }
    }

  };



class Server {

private:

  int port;
  dict::socket::ServerSocket server_socket;
  SOCKET max_known_socket_descriptor;
  fd_set sockets_descriptors_set;
  std::list< std::shared_ptr<dict::socket::ClientSocket> > clients;

public:

  Server(int port=DEFAULT_PORT): server_socket(port) {

    this->port = port;
    FD_ZERO(&this->sockets_descriptors_set);
    FD_SET(
        this->server_socket.get_descriptor(),
        &this->sockets_descriptors_set);
    this->max_known_socket_descriptor =
      this->server_socket.get_descriptor();
    }

  void start(FILE *log) {

    fprintf(log,
        "Server: Started successfully on host \'%s\' – (%s:%d).\n",
        server_socket.get_host_name().c_str(),
        server_socket.get_ip_address().c_str(),
        this->port
        );

    fd_set readable_sockets_set;
    FD_ZERO(&readable_sockets_set);

    while (true) {

      readable_sockets_set = this->sockets_descriptors_set;
      if (SOCKET_ERROR == select(
            this->max_known_socket_descriptor + 1,
            &readable_sockets_set,      // In.
            NULL,                       // Out.
            NULL,                       // Err.
            NULL                        // Lock time.
            )) {
        throw BaseException("Socket error happened. Failed select.");
        }

      if (FD_ISSET(
            this->server_socket.get_descriptor(),
            &sockets_descriptors_set)) {

        // We have new connection.
        this->handle_new_connection(log);
        }

      for (
          std::list<
          std::shared_ptr<dict::socket::ClientSocket>
          >::iterator i = this->clients.begin();
          i != this->clients.end(); i++) {

        if (FD_ISSET(
              (*i)->get_descriptor(),
              &sockets_descriptors_set)) {
          char buffer[MAX_CHAR];
          fprintf(log, "Data from client (length=%d): ",
              (int) (*i)->read(buffer, 1, MAX_CHAR));
          fprintf(log, "%s\n", buffer);
          }

        }

      }
    /**
     * Paleidžia serverį.
     *
     * Kiekvienos iteracijos metu serveris:
     *
     * +  su select susigeneruoja laukiančių klientų aibę A;
     * +  patikrina ar server_socket in A, jei taip į clients_set įkelia
     *    laukiančius klientus;
     * +  keliauja per clients_set ir tuos kurie yra aibėje A, apdoroja.
     */
    }

  void handle_new_connection(FILE *log) {

    std::shared_ptr<dict::socket::ClientSocket> socket =
      server_socket.accept_client();

    this->clients.push_back(socket);
    FD_SET(
        socket->get_descriptor(),
        &this->sockets_descriptors_set);
    this->max_known_socket_descriptor = std::max(
        this->max_known_socket_descriptor,
        socket->get_descriptor());

    socket->write("Sveikas pasauli!\n", 1, 18);
    socket->flush();

    fprintf(
        log, "Client connected: %s.\n",
        socket->get_ip_address().c_str());

    }

  };


};
};

#endif
