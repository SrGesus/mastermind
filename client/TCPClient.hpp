#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <common/UDPSocket.hpp>

class UDPClient {
 private:
  UDPSocket _socket;
  struct addrinfo *_res = nullptr;

 public:
  /// @brief Creates a UDP Client associated with the provided server.
  /// @param ip Ip of the server. Can be null.
  /// @param port Port of the srver. Must not be null.
  UDPClient(const char *ip, const char *port) : _socket() {
    struct addrinfo hints;
    int errcode;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // UDP socket

    errcode = getaddrinfo(ip, port, &hints, &this->_res);
    if (errcode != 0) {
      ERROR("Failed to translate address %s:%s: %s\n",
            ip != nullptr ? ip : "0.0.0.0", port, gai_strerror(errcode));
      exit(1);
    }
  }

  ~UDPClient() { freeaddrinfo(this->_res); }
};

#endif  // UDPCLIENT_HPP_
