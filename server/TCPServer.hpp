#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <common/TCPSocket.hpp>
#include <server/TCPServerParser.hpp>

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

    // Translate address and port
    errcode = getaddrinfo(ip, port, &hints, &res);
    if (errcode != 0)
      ERROR("Failed to translate address %s:%s: %s\n",
            ip != nullptr ? ip : "0.0.0.0", port, gai_strerror(errcode));

    // Bind to address
    errcode = _socket.bind(*(res->ai_addr), res->ai_addrlen);
    if (errcode == -1)
      ERROR("Failed to bind to %s:%s: %s\nIs this port already in use?\n",
            ip != nullptr ? ip : "0.0.0.0", port, strerror(errno));

    // Start
    _socket.listen(QUEUE_SIZE);

    // Avoid Zombie Processes
    struct sigaction act;
    memset(&act, 0, sizeof act);
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGCHLD, &act, NULL) == -1)
      ERROR("Failed to create handler for SIGCHLD: %s\n", strerror(errno));

    freeaddrinfo(res);
  }

  void child(const TCPServerParser &parser) {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    char buf[2048];

    TCPConnection con = _socket.accept((sockaddr &)addr, addrlen);

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    int port = ntohs(addr.sin_port);

    VERBOSE("Received TCP request from %s:%d\n", ip, port);

    con.read_ending(buf, sizeof(buf), '\n');

    const char *result = parser.executeRequest(buf, sizeof(buf));

    DEBUG("Sending back: %s\n", result);

    con.write(result, sizeof(buf));
  }

  void processRequest(const TCPServerParser &parser) {
    int pid;
    if ((pid = fork()) == -1) {
      ERROR("Failed to create Fork.\n");
    } else if (pid == 0) {
      // Child process
      child(parser);
      exit(0);
    } 
    sleep(10);
  }

  /// @return Server's TCP socket.
  TCPSocket &socket() { return _socket; }
};

#endif  // TCPSERVER_HPP_
