#include "package.h"


using namespace dict::package;


Word::Word(std::string value) {
  this->value = value;
  }


Word::Word(std::shared_ptr<dict::socket::ClientSocket> socket) {

  unsigned int buffer_size;

  int result = socket->read(&buffer_size, 4, 1);
  if (result == SOCKET_ERROR) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      throw NoDataException();
      }
    throw BaseException("Error in reading package.");
    }

  buffer_size = ntohl(buffer_size);

  char *buffer = new char[buffer_size + 1];

  char *cursor = buffer;
  int read = 0;

  while (cursor < buffer + buffer_size) {
    read = socket->read_block(cursor, 1, (buffer + buffer_size - cursor));
    if (!read) {
      throw BaseException("Uncomplete package.");
      }
    cursor += read;
    }
  *cursor = 0;

  this->value = std::string(buffer);

  delete []buffer;
  }


std::string Word::get_value() {
  return this->value;
  }


void Word::send(std::shared_ptr<dict::socket::ClientSocket> socket) {

  unsigned int buffer_size = htonl(this->value.length());

  socket->write(&buffer_size, 4, 1);
  socket->write(this->value.c_str(), 1, this->value.length());
  socket->flush();

  }
