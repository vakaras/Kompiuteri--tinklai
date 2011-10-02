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
#include "package.h"


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

    fprintf(
        log, "Server socket descriptor: %d.\n",
        this->server_socket.get_descriptor());

    while (true) {

      fprintf(log, "Server: Selecting.\n");
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

      fprintf(log, "Server: Selected.\n");
      if (FD_ISSET(
            this->server_socket.get_descriptor(),
            &readable_sockets_set)) {
        fprintf(log, "Incomming connection.\n");

        // We have new connection.
        this->handle_new_connection(log);
        }

      fprintf(log, "Checking clients.\n");
      for (
          std::list<
          std::shared_ptr<dict::socket::ClientSocket>
          >::iterator i = this->clients.begin();
          i != this->clients.end(); i++) {

        if (FD_ISSET(
              (*i)->get_descriptor(),
              &sockets_descriptors_set)) {
          try {
            dict::package::Word word(*i);
            fprintf(log, "Searching: %s\n", word.get_value().c_str());
            dict::package::WordList word_list;
            word_list.append("v1", "m1");
            word_list.append("v2", "m2");
            word_list.append("v3", "m3");
            word_list.append("v4", "m4");
            word_list.send(*i);
            fprintf(log, "Result sent.\n");
            }
          catch (dict::package::NoDataException &e) {
            }
          catch (dict::package::BaseException &e) {
            fprintf(
                log,
                "Closing %d connection. Reason: %s.\n",
                (*i)->get_descriptor(),
                e.what());
            FD_CLR((*i)->get_descriptor(), &this->sockets_descriptors_set);
            this->clients.erase(i);
            break;
            }
          }
        fprintf(log, "\n");
        fflush(log);

        }

      }
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

    fprintf(
        log, "Client connected: %s.\n",
        socket->get_ip_address().c_str());

    }

  };


};
};

#endif
