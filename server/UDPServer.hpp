#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <common/UDPSocket.hpp>
#include <server/UDPServerParser.hpp>

class UDPServer {
 private:
  UDPSocket _socket;

 public:
  /// @brief Creates an UDP socket bound to provided ip. Will exit(1) if
  /// unsuccessful.
  /// @param ip Ip to be bound. Can be null.
  /// @param port Port to be bound. Must not be null.
  UDPServer(const char *ip, const char *port) : _socket() {
    struct addrinfo hints, *res = nullptr;
    int errcode;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM;  // UDP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(ip, port, &hints, &res);
    if (errcode != 0)
      ERROR("Failed to translate address %s:%s: %s\n",
            ip != nullptr ? ip : "0.0.0.0", port, gai_strerror(errcode));

    errcode = _socket.bind(*(res->ai_addr), res->ai_addrlen);
    if (errcode == -1)
      ERROR("Failed to bind to %s:%s: %s\nIs this port already in use?\n",
            ip != nullptr ? ip : "0.0.0.0", port, strerror(errno));

    freeaddrinfo(res);
  }

  void processRequest(UDPServerParser &parser) {
    socklen_t addrlen;
    struct sockaddr_in addr;

    const char *result = _socket.recvfrom(addr, addrlen);

    result = parser.executeRequest(result);

    _socket.sendto(result, (sockaddr &)addr, addrlen);
  }
};

#endif  // UDPSERVER_HPP_
