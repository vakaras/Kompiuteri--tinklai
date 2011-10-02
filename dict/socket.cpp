#include "socket.h"


using namespace dict::socket;


ServerSocket::ServerSocket(int port) {

  this->descriptor = -1;
  this->host_entry = NULL;

  this->init_address(port);
  this->init_socket();

  }


ServerSocket::~ServerSocket() {

  close(this->descriptor);

  }


void ServerSocket::init_address(int port) {

  // Getting host name.
  char host_name[MAX_CHAR];
  if (INVALID_SOCKET == gethostname(host_name, sizeof(host_name))) {
    throw BaseException("Failed to get host name.");
    }
  this->host_name = host_name;

  if (NULL == (this->host_entry = gethostbyname(this->host_name.c_str()))) {
    throw BaseException("Failed to get host information.");
    }

  this->address.sin_family = AF_INET;   // AF_INET – internet address 
                                        // family.
  this->address.sin_port = htons(port);
  this->address.sin_addr = *((in_addr *)this->host_entry->h_addr);
  memset(&(this->address.sin_zero), 0, 8);

  }


void ServerSocket::init_socket() {

  if (INVALID_SOCKET == (
        this->descriptor = socket(
          AF_INET,                      // Use IP.
          SOCK_STREAM,                  // Use TCP.
          0))) {
    throw BaseException("Failed to open socket.");
    }

  const int yes = 1;
  if (SOCKET_ERROR == setsockopt(
        this->descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
    throw BaseException("Failed to set address reuse option.");
    }

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


std::string ServerSocket::get_ip_address() {
  return inet_ntoa(*((in_addr *)this->host_entry->h_addr));
  }
