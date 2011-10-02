#ifndef DICT_PACKAGE_H
#define DICT_PACKAGE_H 1


#include <memory>
#include <string>
#include <exception>
#include <sys/param.h>
#include <errno.h>

#include "socket.h"


namespace dict {
namespace package {


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
      return "Packaging exception happened.";
      }
    }
  
  };


class NoDataException: public std::exception {

public:

  virtual const char *what() const throw() {
    return "No data in buffer.";
    }

  };


class Word {

private:

  std::string value;

public:

  Word(std::string value);
  Word(std::shared_ptr<dict::socket::ClientSocket> socket);

  std::string get_value();
  void send(std::shared_ptr<dict::socket::ClientSocket> socket);

  };

};
};

#endif
