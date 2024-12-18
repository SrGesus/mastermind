#ifndef TCPSOCKET_HPP_
#define TCPSOCKET_HPP_

#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <common/TCPConnection.hpp>
#include <common/utils.hpp>

class TCPSocket {
 private:
  int _fd;

  // Delete copy constructor to prevent accidental copies
  TCPSocket(const TCPSocket &) = delete;

 public:
  /// @brief Creates an TCP Socket.
  TCPSocket() {
    _fd = socket(AF_INET, SOCK_STREAM, 0);  // TCP socket
    if (_fd == -1) ERROR("Failed to create TCP Socket: %s\n", strerror(errno));

    // Save process from SIGPIPE
    struct sigaction act;
    memset(&act, 0, sizeof act);
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &act, NULL) == -1)
      ERROR("Failed to create handler for SIGPIPE: %s\n", strerror(errno));
  }

  /// @brief Transforms a tcp socket into a tcp connection.
  /// @param addr Address to be connected to.
  /// @param len Length of address struct.
  /// @return TCP Connection.
  TCPConnection connect(const sockaddr &addr, socklen_t len) && {
    int res = ::connect(_fd, &addr, len);
    if (res == -1) {
      WARN("Failed to create TCP connection to server: %s\n", strerror(errno));
    }
    int fd = _fd;
    _fd = -1;  // Prevents destructor from closing socket.
    return TCPConnection(fd);
  }

  /// @brief Accepts a tcp connection from the listen queue.
  /// @param addr Reference to address struct in which address will be stored.
  /// @param addrlen Reference in which address length will be stored.
  /// @return TCP Connection.
  TCPConnection accept(sockaddr &addr, socklen_t &len) {
    int new_fd = ::accept(_fd, &addr, &len);
    if (new_fd == -1) {
      WARN("Failed to create TCP connection to server: %s\n", strerror(errno));
    }
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
  int bind(const sockaddr *addr, socklen_t len) {
    return ::bind(_fd, addr, len);
  }

  /// @return Socket's file descriptor.
  int fd() { return _fd; }

  ~TCPSocket() {
    if (_fd > 0) {
      int ret;
      do ret = ::close(_fd);
      while (ret == -1 && errno == EINTR);
      DEBUG("TCP Socket was closed.\n");
    }
  }
};

#endif  // TCPSOCKET_HPP_
