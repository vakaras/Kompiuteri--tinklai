#ifndef SMTP_UTILS_H
#define SMTP_UTILS_H


#include <cstdio>
#include <exception>
#include <cstring>
#include <string>


namespace smtp {
namespace utils {


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
      return "Exception in utils occured.";
      }
    }
  
  };


std::string read_file(std::string filename);
  
};
};

#endif
