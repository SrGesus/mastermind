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
  char _buf[BUFFER_SIZE];
  int _fd;

 public:
  /// @brief Constructor from fd.
  /// @param fd Socket fd.
  // /// @note Will exit(1) if fd is -1, so should be on fork.
  TCPConnection(int fd) : _fd(fd) {
    // if (fd == -1) ERROR("Failed to create TCP Socket: %s\n",
    // strerror(errno));
  }


  /// @brief Will read tcp connection into buffer.
  /// @param buf Buffer to be read.
  /// @param siz buffer size
  /// @return Null terminated buffer. If unsuccessful null pointer.
  char * read() {
    int n_read = 0, n;
    do {
      n = ::read(_fd, _buf + n_read, sizeof(_buf) - n_read);
      if (n > 0) n_read += n;
      if (n == -1) {
        if (errno != EINTR) {
          continue;  // Read was interrupted
        }
        WARN("Failed to read to TCP Socket: %s\n", strerror(errno));
        return nullptr;
      }
    } while (n != 0);
    _buf[n_read] = '\0';
    return _buf;
  }

  /// @brief Will write from buffer to tcp connection.
  /// @param buf Contents to be sent, must have at least len chars.
  /// @param len Length to be written.
  /// @note If unsuccessful will exit(1).
  int write(const char* buf, size_t len) {
    int n_written = 0, n;
    do {
      n = ::write(_fd, buf + n_written, len - n_written);
      if (n > 0) n_written += n;
      if (n == -1) {
        if (errno != EINTR) {
          continue;  // Write was interrupted
        }
        WARN("Failed to write to TCP Socket: %s\n", strerror(errno));
        return -1;
      }
    } while (n != 0);
    return n_written;
  }

  ~TCPConnection() {
    DEBUG("TCP Connection was closed.\n");
    close(_fd);
  }
};

#endif  // TCPCONNECTION_HPP_
