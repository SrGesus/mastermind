#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> // HMMMM

#include <common/UDPSocket.hpp>
#include <server/UDPServerParser.hpp>

class UDPServer {
 private:
  UDPSocket _socket;
  bool verbose_flag;

 public:
  #define VERBOSE(...) if (verbose_flag) fprintf(stdout, "[Verbose]: " __VA_ARGS__);  

  /// @brief Creates an UDP socket bound to provided ip. Will exit(1) if
  /// unsuccessful.
  /// @param ip Ip to be bound. Can be null.
  /// @param port Port to be bound. Must not be null.
  /// @param verbose_flag If true, will print debug messages.
  UDPServer(const char *ip, const char *port, bool verbose_flag) : _socket(), verbose_flag(verbose_flag) {
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

    const char *result = _socket.recvfrom(&addr, &addrlen);

    char ip[INET_ADDRSTRLEN]; // HMMMM
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    int port = ntohs(addr.sin_port);

    VERBOSE("Received %s from %s:%d\n", result, ip, port);

    result = parser.executeRequest(result);

    _socket.sendto(result, (sockaddr &)addr, addrlen);
  }

  /// @return Server's UDP socket.
  UDPSocket &socket() { return _socket; }
};

#endif  // UDPSERVER_HPP_
