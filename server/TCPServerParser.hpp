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
      VERBOSE_APPEND("\tType: Show Trials\n");
      VERBOSE_APPEND("\tPLID: %06d\n", plid);
      GameSession &game = _gameStore.getSession(plid);
      if (!game.exists()) {
        VERBOSE_APPEND("\tResult: Could not find game.\n");
        return "STR NOK\n";
      }
      const char *status = game.inProgress() ? "ACT" : "FIN";

      std::string Fdata = game.showTrials(plid);
      VERBOSE_APPEND("\tResult: Showing Trials: \n%s\n", Fdata.c_str());

      sprintf(req, "RST %s TRIALS_%06d.txt %lu %s\n", status, plid,
              Fdata.size(), Fdata.c_str());
      return req;
    }

    if (strncmp(req, "SSB", 3) == 0) {
      std::string Fdata = _gameStore.getScoreboardString();
      VERBOSE_APPEND("\tType: Show Scoreboard\n");
      if (Fdata.empty()) {
        VERBOSE_APPEND("\tResult: Scoreboard is empty.\n");
        return "RSS EMPTY\n";
      }
      const time_t now = time(nullptr);
      char timeStr[15];
      strftime(timeStr, sizeof(timeStr), "%Y%m%d%H%M%S", localtime(&now));

      VERBOSE_APPEND(
          "\tResult: Showing Scoreboard SCORES%14s.txt (%lu bytes): \n%s\n",
          timeStr, Fdata.size(), Fdata.c_str());
      sprintf(req, "RSS OK SCORES%14s.txt %lu %s\n", timeStr, Fdata.size(),
              Fdata.c_str());
      return req;
    }
    VERBOSE_APPEND("\tResult: Couldn't process request: %s", req);
    return "ERR\n";
  }
};

#endif  // TCPSERVERPARSER_HPP_