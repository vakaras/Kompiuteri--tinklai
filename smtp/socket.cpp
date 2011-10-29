#include "socket.h"


using namespace dict::socket;


void Socket::set_reuse_option() {

  const int yes = 1;
  if (SOCKET_ERROR == setsockopt(
        this->descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
    throw BaseException("Failed to set address reuse option.");
    }

  }


void Socket::get_host_by_name(const char *host_name) {

  if (NULL == (this->host_entry = gethostbyname(host_name))) {
    throw BaseException("Failed to get host information.");
    }

  }


void Socket::form_address(int port) {

  //this->address.sin_len = sizeof(this->address);
  this->address.sin_family = AF_INET;   // AF_INET – internet address 
                                        // family.
  this->address.sin_port = htons(port);
  this->address.sin_addr = *((in_addr *)this->host_entry->h_addr);
  memset(&(this->address.sin_zero), 0, 8);

  }


void Socket::open_socket() {

  if (INVALID_SOCKET == (
        this->descriptor = ::socket(
          AF_INET,                      // Use IP.
          SOCK_STREAM,                  // Use TCP.
          0))) {
    throw BaseException("Failed to open socket.");
    }

  }


std::string Socket::get_ip_address() {
  return inet_ntoa(*((in_addr *)this->host_entry->h_addr));
  }


SOCKET Socket::get_descriptor() {
  return this->descriptor;
  }


ClientSocket::ClientSocket(SOCKET server_socket_descriptor) {

  unsigned int address_size = sizeof(sockaddr_in);

  if (SOCKET_ERROR == (this->descriptor = accept(
          server_socket_descriptor,
          (sockaddr *) &this->address,
          &address_size))) {
    throw BaseException("Failed to open client socket.");
    }

  this->set_reuse_option();

  if (NULL == (this->host_entry = gethostbyaddr(
          (void *) &this->address.sin_addr,
          sizeof(this->address.sin_addr),
          AF_INET))) {

    close(this->descriptor);
    throw BaseException("Failed to get host information.");
    }

  this->read_connection = fdopen(this->descriptor, "r");
  this->write_connection = fdopen(this->descriptor, "w");

  }


ClientSocket::ClientSocket(const char *host_name, int port) {

  this->get_host_by_name(host_name);
  this->form_address(port);
  this->open_socket();
  this->set_reuse_option();

  if (SOCKET_ERROR == connect(
        this->descriptor,
        (sockaddr *) &this->address,
        sizeof(this->address))) {
    throw BaseException("Connection failed!");
    }

  this->read_connection = fdopen(this->descriptor, "r");
  this->write_connection = fdopen(this->descriptor, "w");

  }


ClientSocket::~ClientSocket() {
  fflush(this->write_connection);
  fclose(this->write_connection);
  fclose(this->read_connection);
  }


// Non blocking read.
size_t ClientSocket::read(void *buffer, size_t size, size_t count) {
  return recv(this->descriptor, buffer, count * size, MSG_DONTWAIT);
  }


// Blocking read.
size_t ClientSocket::read_block(void *buffer, size_t size, size_t count) {
  return recv(this->descriptor, buffer, count * size, 0);
  }


int ClientSocket::write(const void *buffer, size_t size, size_t count) {
  return fwrite(buffer, size, count, this->write_connection);
  }


void ClientSocket::flush() {
  fflush(this->write_connection);
  }


FILE *ClientSocket::get_read_connection() {
  return this->read_connection;
  }


FILE *ClientSocket::get_write_connection() {
  return this->write_connection;
  }


ServerSocket::ServerSocket(int port) {

  this->descriptor = -1;
  this->host_entry = NULL;

  this->init_address(port);
  this->init_socket();

  }


Socket::~Socket() {

  close(this->descriptor);

  }


void ServerSocket::init_address(int port) {

  // Getting host name.
  char host_name[MAX_CHAR];
  if (INVALID_SOCKET == gethostname(host_name, sizeof(host_name))) {
    throw BaseException("Failed to get host name.");
    }
  this->host_name = host_name;

  this->get_host_by_name(host_name);
  this->form_address(port);

  }


void ServerSocket::init_socket() {

  this->open_socket();
  this->set_reuse_option();

  if (SOCKET_ERROR == bind(
        this->descriptor,
        (sockaddr *)&this->address,
        sizeof(this->address))) {

    close(this->descriptor);
    throw BaseException("Failed to bind socket.");
    }

  if (SOCKET_ERROR == listen(this->descriptor, MAX_WAITING_CONNECTIONS)) {
    close(this->descriptor);
    throw BaseException("Failed to start listening.");
    }

  }


std::string ServerSocket::get_host_name() {
  return this->host_name;
  }


std::shared_ptr<ClientSocket> ServerSocket::accept_client() {
  return std::shared_ptr<ClientSocket>(new ClientSocket(this->descriptor));
  }
