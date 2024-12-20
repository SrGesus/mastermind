#ifndef UDPSERVERPARSER_HPP_
#define UDPSERVERPARSER_HPP_

#include <string.h>

#include <common/utils.hpp>
#include <server/GameStorage.hpp>
#include <server/Trial.hpp>

class UDPServerParser {
 private:
  char _buf[BUFFER_SIZE];
  GameStorage &_gameStore;

 public:
  UDPServerParser(GameStorage &sessions) : _gameStore(sessions) {
    srand(time(NULL));
  }

  const char *executeRequest(const char *req) {
    // Start New Game
    if (strncmp(req, "SNG", 3) == 0) {
      int plid, maxTime;
      char newLine;
      if ((sscanf(req, "SNG %06d %03d%c", &plid, &maxTime, &newLine) != 3) ||
          (plid < 1 || plid > 999999 || maxTime < 1 || maxTime > 600) ||
          newLine != '\n') {
        VERBOSE_APPEND("\tResult: Couldn't process request: %s", req);
        return "RSG ERR\n";
      }
      VERBOSE_APPEND("\tType: Start New Game\n");
      VERBOSE_APPEND("\tPLID: %06d\n", plid);
      VERBOSE_APPEND("\tMaxTime: %3d\n", maxTime);

      GameSession &game = _gameStore.getSession(plid);
      if (game.inProgress() && game.nT() > 1) {
        // There's already a game in Progress.
        VERBOSE_APPEND("\tResult: Game already in progress.\n");
        return "RSG NOK\n";
      }

      // Start a new game.
      game = GameSession::newGame(maxTime);
      _gameStore.newSession(plid, game);
      char c1 = game.getCode().c1(), c2 = game.getCode().c2(),
           c3 = game.getCode().c3(), c4 = game.getCode().c4();
      VERBOSE_APPEND("\tResult: Started New Game with code %c %c %c %c.\n", c1,
                     c2, c3, c4);
      return "RSG OK\n";
    }

    // Try a guess
    if (strncmp(req, "TRY", 3) == 0) {
      int plid, nT, res;
      char c1, c2, c3, c4, newLine;

      // Check
      if ((sscanf(req, "TRY %06d %c %c %c %c %d%c", &plid, &c1, &c2, &c3, &c4,
                  &nT, &newLine) != 7) ||
          (plid < 1 || plid > 999999)) {
        VERBOSE_APPEND("\tResult: Couldn't process request: %s", req);
        return "RTR ERR\n";
      }
      VERBOSE_APPEND("\tType: Try\n");
      VERBOSE_APPEND("\tPLID: %06d\n", plid);
      VERBOSE_APPEND("\tTrial: %c %c %c %c\n", c1, c2, c3, c4);
      VERBOSE_APPEND("\tnT: %d\n", nT);
      GameSession &game = _gameStore.getSession(plid);
      if (!game.exists()) {
        // There is no game for this PLID.
        VERBOSE_APPEND("\tResult: There's currently no game in progress.\n");
        return "RTR NOK\n";
      }

      Trial t(c1, c2, c3, c4);
      uint16_t nB = 0, nW = 0;
      const Trial &code = game.getCode();
      res = game.executeTrial(t, nT, nB, nW);

      switch (res) {
        case GameSession::TrialResult::ERROR:
          VERBOSE_APPEND("\tResult: Couldn't process request: %s", req);
          return "RTR ERR\n";
        case GameSession::TrialResult::QUIT:
          VERBOSE_APPEND("\tResult: There's currently no game in progress.\n");
          return "RTR NOK\n";
        case GameSession::TrialResult::DUPLICATE:
          VERBOSE_APPEND(
              "\tResult: Same attempt was already made this session.\n");
          return "RTR DUP\n";
        case GameSession::TrialResult::INVALID:
          VERBOSE_APPEND("\tResult: Invalid trial number.\n");
          return "RTR INV\n";
        case GameSession::TrialResult::TIMEOUT:
          VERBOSE_APPEND("\tResult: Time Limit Exceeded.\n");
          sprintf(_buf, "RTR ETM %c %c %c %c\n", code.c1(), code.c2(),
                  code.c3(), code.c4());
          return _buf;
        case GameSession::TrialResult::LOSS:
          VERBOSE_APPEND("\tResult: Limit of Tries Exceeded.\n");
          sprintf(_buf, "RTR ENT %c %c %c %c\n", code.c1(), code.c2(),
                  code.c3(), code.c4());
          return _buf;
        case GameSession::TrialResult::WIN:
          VERBOSE_APPEND("\tResult: Victory!\n");
          _gameStore.addToScoreboard(plid, game);
          sprintf(_buf, "RTR OK %d %d %d\n", nT, nB, nW);
          return _buf;
        case GameSession::TrialResult::PLAYING:
          VERBOSE_APPEND("\tResult: nB=%d nW=%d.\n", nB, nW);
          sprintf(_buf, "RTR OK %d %d %d\n", nT, nB, nW);
          return _buf;
      }
    }

    // Quit game
    if (strncmp(req, "QUT", 3) == 0) {
      int plid;
      char newLine;

      if ((sscanf(req, "QUT %06d%c", &plid, &newLine) != 2) ||
          (plid < 1 || plid > 999999) || newLine != '\n') {
        VERBOSE_APPEND("\tResult: Couldn't process request: %s", req);
        return "RQT ERR\n";
      }
      VERBOSE_APPEND("\tType: Quit\n");
      VERBOSE_APPEND("\tPLID: %06d\n", plid);
      GameSession &game = _gameStore.getSession(plid);
      // Attempt to end game
      if (!game.endGame()) {
        VERBOSE_APPEND("\tResult: There's currently no game in progress.\n");
        return "RQT NOK\n";
      }
      VERBOSE_APPEND("\tResult: Quit game.\n");
      const Trial &code = game.getCode();
      sprintf(_buf, "RQT OK %c %c %c %c\n", code.c1(), code.c2(), code.c3(),
              code.c4());
      return _buf;
    }

    // Start new Game with given secret
    if (strncmp(req, "DBG", 3) == 0) {
      int plid, maxTime;
      char c1, c2, c3, c4, newLine;
      if ((sscanf(req, "DBG %06d %03d %c %c %c %c%c", &plid, &maxTime, &c1, &c2,
                  &c3, &c4, &newLine) != 7) ||
          (plid < 1 || plid > 999999 || maxTime < 1 || maxTime > 600) ||
          newLine != '\n') {
        VERBOSE_APPEND("\tResult: Couldn't process request: %s", req);
        return "RDB ERR\n";
      }
      Trial code = Trial(c1, c2, c3, c4);
      if (!code.isValid()) {
        VERBOSE_APPEND("\tResult: Couldn't process request: %s", req);
        return "RDB ERR\n";
      }
      VERBOSE_APPEND("\tType: Start New Debug Game\n");
      VERBOSE_APPEND("\tPLID: %06d\n", plid);
      VERBOSE_APPEND("\tMaxTime: %3d\n", maxTime);
      VERBOSE_APPEND("\tCode: %c %c %c %c\n", c1, c2, c3, c4);
      if (_gameStore.getSession(plid).inProgress()) {
        // There's already a game in Progress.
        VERBOSE_APPEND("\tResult: Game already in Progress.\n");
        return "RDB NOK\n";
      }
      // Start a new game.
      VERBOSE_APPEND("\tResult: Started New Game with code %c %c %c %c.\n", c1,
                     c2, c3, c4);
      _gameStore.newSession(plid, GameSession::newDebugGame(maxTime, code));
      return "RDB OK\n";
    }
    VERBOSE_APPEND("\tResult: Couldn't process request: %s", req);
    return "ERR\n";
  }
};

#endif  // UDPSERVERPARSER_HPP_
