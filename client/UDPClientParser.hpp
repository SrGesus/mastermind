#ifndef UDPCLIENTPARSER_HPP_
#define UDPCLIENTPARSER_HPP_

#include <common/utils.hpp>

/// @brief Has the capacity to parse Server responses and handle them
class UDPClientParser {
 private:
  char _buf[BUFFER_SIZE];

 public:
  const char *executeRequest(const char *req) { return _buf; }
};

#endif  // UDPCLIENTPARSER_HPP_
