#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <common/TCPSocket.hpp>
// #include <server/TCPServerParser.hpp>

class TCPServer {
 private:
  TCPSocket _socket;

 public:
  /// @brief Size of TCP listen queue.
  static const int QUEUE_SIZE = 10;

  /// @brief Creates an TCP socket bound to provided ip. Will exit(1) if
  /// unsuccessful.
  /// @param ip Ip to be bound. Can be null.
  /// @param port Port to be bound. Must not be null.
  TCPServer(const char *ip, const char *port) : _socket() {
    struct addrinfo hints, *res = nullptr;
    int errcode;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(ip, port, &hints, &res);
    if (errcode != 0)
      ERROR("Failed to translate address %s:%s: %s\n",
            ip != nullptr ? ip : "0.0.0.0", port, gai_strerror(errcode));

    errcode = _socket.bind(*(res->ai_addr), res->ai_addrlen);
    if (errcode == -1)
      ERROR("Failed to bind to %s:%s: %s\nIs this port already in use?\n",
            ip != nullptr ? ip : "0.0.0.0", port, strerror(errno));

    _socket.listen(QUEUE_SIZE);

    freeaddrinfo(res);
  }

  // void processRequest(const TCPServerParser &parser) {
  //   struct sockaddr_in addr;
  //   socklen_t addrlen = sizeof(addr);
  //   char buf[BUFFER_SIZE];

  //   TCPConnection con = _socket.accept((sockaddr &)addr, addrlen);

  //   con.read(buf, sizeof(buf));

  //   char * result = parser.executeRequest(buf);

  //   con.write(result, );
  // }

  /// @return Server's TCP socket.
  TCPSocket &socket() { return _socket; }
};

#endif  // TCPSERVER_HPP_
