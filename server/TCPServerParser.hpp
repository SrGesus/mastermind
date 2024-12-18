#ifndef TCPSERVERPARSER_HPP_
#define TCPSERVERPARSER_HPP_

#include <cstring>

#include "GameStorage.hpp"

class TCPServerParser {
 private:
  GameStorage &_gameStore;

 public:
  TCPServerParser(GameStorage &sessions) : _gameStore(sessions) {}

  const char *executeRequest(char *req, size_t size) const {
    int plid;
    char newLine;
    if (strncmp(req, "STR", 3) == 0) {
      if (sscanf(req, "STR %06d%c", &plid, &newLine) != 2 || plid < 1 ||
          plid > 999999 || newLine != '\n') {
        return "STR NOK\n";
      }
      GameSession &game = _gameStore.getSession(plid);
      if (!game.exists()) {
        DEBUG("Doesn't exist lol\n")
        return "STR NOK\n";
      }
      const char *status = game.inProgress() ? "ACT" : "FIN";

      std::string Fdata = game.showTrials();

      
      
    }
    return "ERR\n";
  }
};

#endif  // TCPSERVERPARSER_HPP_