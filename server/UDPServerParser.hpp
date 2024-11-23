#ifndef UDPSERVERPARSER_HPP_
#define UDPSERVERPARSER_HPP_

#include <common/utils.hpp>
#include <common/Trial.hpp>
#include <string.h>

class UDPServerParser {
 private:
  char _buf[BUFFER_SIZE];
  bool validColor(char c) {
    switch (c) {
      case Color::red:
      case Color::green:
      case Color::blue:
      case Color::yellow:
      case Color::orange:
      case Color::purple:
        return true;
      default:  // Invalid color
        return false;
    }
  }
  // 
  bool validCode(const char *code) {
    if (strlen(code) != 7) {
      return false;
    }
    for (size_t i = 0; i < 4; i++) {
      if (!validColor(code[i])) {
        return false;
      }
    }
    return true;
  }


 public:
  const char *executeRequest(const char *req) { 

    if (strncmp(req, "SNG", 3) == 0) {
      int plid, maxTime;
      if ((sscanf(req, "SNG %06d %03d\n", &plid, &maxTime) != 2) || (plid < 1 || plid > 999999 || maxTime < 1 || maxTime > 600)) {
        sprintf(_buf, "RSG ERR\n");
        return _buf;
      }
      // EXECUTE SNG
      sprintf(_buf, "RSG OK\n");

    }

    if (strncmp(req, "TRY", 3) == 0) {
      int plid, nT;
      char guess[8];

      if ((sscanf(req, "TRY %06d %s %nT\n", &plid, &guess, &nT) != 2) || (plid < 1 || plid > 999999) or (strlen(guess) != 7)) {
        sprintf(_buf, "RTR ERR\n");

      }

      Trial t = Trial(guess, 7);
      // EXECUTE TRY
      sprintf(_buf, "RQT OK\n");

    }

    if (strncmp(req, "QUT", 3) == 0) {
      int plid;
      if ((sscanf(req, "QUT %06d\n", &plid) != 1) || (plid < 1 || plid > 999999)) {
        sprintf(_buf, "RQT NOK\n");

      }
      // EXECUTE QUT
      sprintf(_buf, "RQT OK\n");

    }

    if (strncmp(req, "DBG", 3) == 0) {
      int plid, maxTime;
      char key[8];

      if ((sscanf(req, "DBG %06d %03d %s\n", &plid, &maxTime, &key) != 4) || (plid < 1 || plid > 999999 || maxTime < 1 || maxTime > 600) || !validCode(key)) {
        sprintf(_buf, "RDB ERR\n");

      }
      // EXECUTE DBG
      sprintf(_buf, "RDB OK\n");

    }

    WARN("Request: %s\n", req);
    WARN("Return: %s\n", _buf);
    return _buf;
  }

};

#endif  // UDPSERVERPARSER_HPP_
