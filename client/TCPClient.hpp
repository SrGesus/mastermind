#ifndef TCPCLIENT_HPP_
#define TCPCLIENT_HPP_

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <common/TCPSocket.hpp>

class TCPClient {
 private:
  struct addrinfo *_res = nullptr;
  char _buf[4096];

 public:
  /// @brief Max number of retries for commands
  static const int MAX_RETRIES = 4;

  /// @brief Creates a TCP Client associated with the provided server.
  /// @param ip Ip of the server. Can be null.
  /// @param port Port of the srver. Must not be null.
  TCPClient(const char *ip, const char *port) {
    struct addrinfo hints;
    int errcode;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP socket

    errcode = getaddrinfo(ip, port, &hints, &_res);
    if (errcode != 0) {
      ERROR("Failed to translate address %s:%s: %s\n",
            ip != nullptr ? ip : "0.0.0.0", port, gai_strerror(errcode));
      exit(1);
    }
  }

  /// @brief Sends command to server and returns response.
  /// @param req Null terminated request to be sent.
  /// @return The buffer in which the socket will write the server's response.
  /// Returns "ERR\\n" if Maximum retries is exceeded.
  /// @note Returned Buffer will be overwritten if socket is read from again.
  const char *runCommand(const char *req) {
    TCPConnection con = TCPSocket().connect(*_res->ai_addr, _res->ai_addrlen);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(con.fd(), &set);

    int retries = 0;
    while (retries <= MAX_RETRIES) {
      timeval timeout = {.tv_sec = 1, .tv_usec = 0};
      // Add 400ms of timeout for every retry.
      timeout.tv_usec += retries * 400000;

      DEBUG("Sending via TCP: %s", req);

      // Send command to server.
      con.write(req, strlen(req));

      // Wait for socket to be readable.
      switch (select(con.fd() + 1, &set, nullptr, nullptr, &timeout)) {
        case 0:  // Timed out
          DEBUG("Timed out waiting for TCP server response, retrying...\n");
          retries++;
          continue;
        case -1:  // Unexpected Error
          DEBUG("Could not get reply from server: %s\n", strerror(errno));
          return ERR_RESPONSE;
        default:                               // Handle message
          if (con.read(_buf, sizeof(_buf)) == -1) {  // Unexpected Error
            DEBUG("Could not get reply from server: %s\n", strerror(errno));
            return ERR_RESPONSE;
          }
          DEBUG("Received via TCP: %s", _buf);
          return _buf;
      }
    }
    WARN("Could not get reply from server: Maximum retries (%d) exceeded.\n",
          MAX_RETRIES);
    return ERR_RESPONSE;
  }

  ~TCPClient() {
    DEBUG("TCP Client was destroyed.\n");
    freeaddrinfo(_res);
  }
};

#endif  // TCPCLIENT_HPP_
