#ifndef TCPCONNECTION_HPP_
#define TCPCONNECTION_HPP_

#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <common/utils.hpp>

/// @brief Represents a TCP Connection after a server accepts or a client
/// connects.
class TCPConnection {
 private:
  int _fd;
  fd_set _set;

  // Delete copy constructor to prevent accidental copies
  TCPConnection(const TCPConnection &) = delete;
  TCPConnection &operator=(const TCPConnection &) = delete;

 public:
  /// @brief Constructor from fd.
  /// @param fd Socket fd.
  TCPConnection(int fd) : _fd(fd) {
    FD_ZERO(&_set);
    FD_SET(_fd, &_set);
  }

  /// @brief Will read tcp connection into buffer, until ending char is found.
  /// @param buf Buffer to be read.
  /// @param len Length to be read.
  /// @param ending character that represents the end.
  /// @return Number of bytes read. If unsucessful -1.
  int read(char *buf, int len, char ending = '\0') {
    int n_read = 0, n;
    timeval timeout = {.tv_sec = 0, .tv_usec = 800000};
    do {
      n = ::read(_fd, buf + n_read, len - n_read);
      if (n > 0) {
        n_read += n;
        // If reading ends with provided char, then we can stop.
        if (buf[n_read - 1] == ending) break;
      }
      if (n == -1) {
        if (errno == EINTR) {
          continue;  // Read was interrupted
        }
        WARN("Failed to read to TCP Socket: %s\n", strerror(errno));
        buf[0] = '\0';
        return -1;
      }
      // If after reading and waiting timeout nothing else is read
      // stop reading further.
    } while (n != 0 && select(_fd + 1, &_set, nullptr, nullptr, &timeout) != 0);
    buf[n_read] = '\0';
    return n_read;
  }

  /// @brief Will write from buffer to tcp connection.
  /// @param buf Contents to be sent, must have at least len chars.
  /// @param len Length to be written.
  /// @return Number of bytes read. If unsucessful -1.
  int write(const char *buf, int len) {
    int n_written = 0, n;
    do {
      n = ::write(_fd, buf + n_written, len - n_written);
      if (n > 0) n_written += n;
      if (n == -1) {
        if (errno == EINTR) {
          continue;  // Write was interrupted
        }
        WARN("Failed to write to TCP Socket: %s\n", strerror(errno));
        return -1;
      }
    } while (n != 0);
    return n_written;
  }

  /// @return Socket's file descriptor.
  int fd() { return _fd; }

  ~TCPConnection() {
    if (_fd != -1) {
      close(_fd);
      DEBUG("TCP Connection was closed.\n");
    }
  }
};

#endif  // TCPCONNECTION_HPP_
