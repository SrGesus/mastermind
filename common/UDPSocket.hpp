#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <utils.hpp>

// Revisit this value
#define BUFFER_SIZE 128

class UDPSocket {
 private:
  char _buf[BUFFER_SIZE];

 protected:
  int _fd;

 public:
  /// @brief Creates an UDP Socket.
  UDPSocket() {
    this->_fd = socket(AF_INET, SOCK_DGRAM, 0);  // UDP socket
    if (this->_fd == -1) {
      ERROR("Failed to create Socket: %s\n", strerror(errno));
      exit(1);
    }
  }

  /// @brief Sends null-terminated data to provided address.
  /// @param in Null-terminated data to be sent.
  /// @param addr Socket address.
  /// @param addrlen Socket address length.
  /// @note If unsuccessful will print a warning.
  void sendto(const char *in, struct sockaddr &addr, socklen_t addrlen) {
    size_t n = strlen(in);
    ssize_t n_sent = ::sendto(_fd, in, n, 0, (struct sockaddr *)&addr, addrlen);
    if (n_sent != n)
      WARN("UDP Failed to send %zu bytes: %s\n", n, strerror(errno));
  }

  /// @brief Receives
  /// @param addr Reference to address struct in which address will be stored.
  /// @param addrlen Reference in which address length will be stored.
  /// @note If unsuccessful will print a warning.
  const char *recvfrom(struct sockaddr &addr, socklen_t &addrlen) {
    ssize_t n_recv = ::recvfrom(_fd, _buf, BUFFER_SIZE - 1, 0, &addr, &addrlen);
    if (n_recv == -1) {
      WARN("UDP Failed to receive:\n", strerror(errno));
      n_recv = 0;
    }
    _buf[n_recv] = '\0';
    return _buf;
  }

  /// @brief Wrapper for bind from <sys/socket.h>
  /// @param addr Address struct
  /// @param len Address length
  /// @return On success, zero is returned.  On error, -1 is returned, and errno
  /// is set to indicate the error.
  int bind(const sockaddr *addr, socklen_t len) {
    return ::bind(_fd, addr, len);
  }
};

#undef BUFFER_SIZE
