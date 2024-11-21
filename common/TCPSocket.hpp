#ifndef TCPSOCKET_HPP_
#define TCPSOCKET_HPP_

#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <common/TCPConnection.hpp>
#include <common/utils.hpp>

class TCPSocket {
 private:
  int _fd;

 public:
  /// @brief Creates an TCP Socket.
  TCPSocket() {
    _fd = socket(AF_INET, SOCK_STREAM, 0);  // TCP socket
    if (_fd == -1) ERROR("Failed to create TCP Socket: %s\n", strerror(errno));
  }

  TCPConnection connect(const sockaddr &addr, socklen_t len) {
    int new_fd = ::connect(_fd, &addr, len);
    return TCPConnection(new_fd);
  }

  TCPConnection accept(sockaddr &addr, socklen_t &len) {
    int new_fd = ::accept(_fd, &addr, &len);
    return TCPConnection(new_fd);
  }

  /// @brief Wrapper for listen from <sys/socket.h> for TCP.
  /// @param n Size of connections queue.
  void listen(int n) {
    if (::listen(_fd, n) == -1)
      ERROR("Failed to listen on TCP Socket: %s\n", strerror(errno));
  }

  /// @brief Wrapper for bind from <sys/socket.h>
  /// @param addr Address struct
  /// @param len Address length
  /// @return On success, zero is returned. On error, -1 is returned, and errno
  /// is set to indicate the error.
  int bind(const sockaddr &addr, socklen_t len) {
    return ::bind(_fd, &addr, len);
  }

  /// @brief Adds socket to fd_set.
  /// @param set fd_set to be used.
  void set(fd_set &set) { FD_SET(_fd, &set); }

  /// @brief Checks if socket is active.
  /// @param set fd_set to be used.
  bool isSet(const fd_set &set) { return FD_ISSET(_fd, &set); }

  ~TCPSocket() { close(_fd); }
};

#endif  // TCPSOCKET_HPP_
