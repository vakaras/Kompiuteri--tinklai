#ifndef SMTP_MIME_H
#define SMTP_MIME_H 1


#include <string>
#include <map>
#include <list>
#include "encoders.h"
#include "utils.h"


namespace smtp {
namespace mime {


class MIMEBase {

private:

  std::map<std::string, std::string> headers;

public:

  void add_header(std::string type, std::string value);
  void add_utf8_header(std::string type, std::string value);
  void write_headers(FILE *fp);
  virtual void write(FILE *fp) = 0;

  };


class MIMEHTML: public MIMEBase {

private:

  std::string filename;

public:

  MIMEHTML(std::string filename);
  void write(FILE *fp);

  };


class MIMEImage: public MIMEBase {

private:

  std::string filename;

public:

  MIMEImage(std::string filename, std::string content_id);
  void write(FILE *fp);
  
  };


class MIMEMultipart: public MIMEBase {

private:

  MIMEBase *parts[20];
  int size;

public:

  MIMEMultipart(std::string subject, std::string recipient);
  void write(FILE *fp);
  void append(MIMEBase *part);

  };

};
};

#endif
