#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

//
#include <common/UDPSocket.hpp>

const char *ERR_RESPONSE = "ERR\n";

class UDPClient {
 private:
  UDPSocket _socket;
  struct addrinfo *_res = nullptr;

 public:
  /// @brief Max number of retries for commands
  static const int MAX_RETRIES = 10;

  /// @brief Creates a UDP Client associated with the provided server.
  /// @param ip Ip of the server. Can be null.
  /// @param port Port of the srver. Must not be null.
  UDPClient(const char *ip, const char *port) : _socket() {
    struct addrinfo hints;
    int errcode;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM;  // UDP socket

    errcode = getaddrinfo(ip, port, &hints, &this->_res);
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
    fd_set set;
    FD_ZERO(&set);
    FD_SET(_socket.fd(), &set);

    int retries = 0;

    while (retries <= MAX_RETRIES) {
      timeval timeout = {.tv_sec = 1, .tv_usec = 0};
      // Add 200ms of timeout for every retry.
      timeout.tv_usec += retries * 200000;

      DEBUG("Sending via UDP: %s", req);

      // Send command to server.
      _socket.sendto(req, *_res->ai_addr, _res->ai_addrlen);

      // Wait for socket to be readable.
      switch (select(_socket.fd() + 1, &set, nullptr, nullptr, &timeout)) {
        case 0:  // Timed out
          DEBUG("Timed out waiting for UDP server response, retrying...\n");
          retries++;
          continue;
        case -1:  // Unexpected Error
          DEBUG("Could not get reply from server: %s\n", strerror(errno));
          return ERR_RESPONSE;
        default:  // Handle message
          char *resp = _socket.recvfrom(nullptr, nullptr);
          if (resp == nullptr) {  // Unexpected Error
            DEBUG("Could not get reply from server: %s\n", strerror(errno));
            return ERR_RESPONSE;
          }
          DEBUG("Received via UDP: %s", resp);
          return resp;
      }
    }
    DEBUG("Could not get reply from server: Maximum retries (%d) exceeded.\n",
           MAX_RETRIES);
    return ERR_RESPONSE;
  }

  ~UDPClient() { 
    DEBUG("UDP Client was destroyed.\n");
    freeaddrinfo(this->_res);
  }
};

#endif  // UDPCLIENT_HPP_
