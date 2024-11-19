#ifndef UDPSERVERPARSER_HPP_
#define UDPSERVERPARSER_HPP_

#include <common/utils.hpp>

class UDPServerParser {
 private:
  char _buf[BUFFER_SIZE];

 public:
  const char *executeRequest(const char *req) {
    return _buf;
  }
};

#endif  // UDPSERVERPARSER_HPP_