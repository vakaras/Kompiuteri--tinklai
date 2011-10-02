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
    throw BaseException("Error in reading word length.");
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
    else if (read == SOCKET_ERROR) {
      throw BaseException("Error in reading package from socket.");
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


WordList::WordList() {

  this->words = std::shared_ptr< std::list<Word> >(new std::list<Word>);

  }

WordList::WordList(std::shared_ptr<dict::socket::ClientSocket> socket) {

  this->words = std::shared_ptr< std::list<Word> >(new std::list<Word>);

  unsigned int word_count;
  if (SOCKET_ERROR == socket->read_block(&word_count, 4, 1)) {
    throw BaseException("Error in reading word count.");
    }
  word_count = ntohl(word_count);

  for (int i = 0; i < word_count; i++) {

    this->append(this->read_string(socket), this->read_string(socket));
    
    }

  }


std::string WordList::read_string(
      std::shared_ptr<dict::socket::ClientSocket> socket) {

  unsigned int buffer_size;

  if (SOCKET_ERROR == socket->read_block(&buffer_size, 4, 1)) {
    throw BaseException("Error in reading value length.");
    }

  buffer_size = ntohl(buffer_size);
  char *buffer = new char[buffer_size + 1];
  char *cursor = buffer;
  for (int read = 0; cursor < buffer + buffer_size; cursor += read) {

      read = socket->read_block(
          cursor, 1, (buffer + buffer_size - cursor));

      if (!read) {
        throw BaseException("Uncomplete package.");
        }
      else if (read == SOCKET_ERROR) {
        throw BaseException("Error in reading package from socket.");
        }

      }
  *cursor = 0;

  std::string result = buffer;
  delete []buffer;
  return result;
  }


void WordList::write_string(
    std::shared_ptr<dict::socket::ClientSocket> socket, std::string value) {

  unsigned int buffer_size = htonl(value.length());

  socket->write(&buffer_size, 4, 1);
  socket->write(value.c_str(), 1, value.length());
  
  }


void WordList::send(std::shared_ptr<dict::socket::ClientSocket> socket) {

  unsigned int word_count = htonl(this->words->size());

  socket->write(&word_count, 4, 1);

  for (std::list<Word>::iterator i = this->words->begin();
      i != this->words->end(); i++) {

    this->write_string(socket, i->value);
    this->write_string(socket, i->meaning);

    }
  socket->flush();

  }


void WordList::append(std::string value, std::string meaning) {
  this->words->push_back(Word(value, meaning));
  }


std::shared_ptr< std::list<WordList::Word> > WordList::get_words() {
  return this->words;
  }
