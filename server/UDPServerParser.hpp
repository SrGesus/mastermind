#ifndef UDPSERVERPARSER_HPP_
#define UDPSERVERPARSER_HPP_

#include <common/utils.hpp>
#include <common/Trial.hpp>
#include <server/GameStorage.hpp>
#include <string.h>

class UDPServerParser {
  private:
  char _buf[BUFFER_SIZE];
  GameStorage *_sessions;

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
    printf("validCode: %s\n", code);
    if (strlen(code) != 7) {
      return false;
    }
    for (size_t i = 0; i < 7; i+=2) {
      if (!validColor(code[i])) {
        return false;
      }
    }
    return true;
  }


  public:
  UDPServerParser(GameStorage *sessions) : _sessions(sessions)  {}

  const char *executeRequest(const char *req) {
    if (strncmp(req, "SNG", 3) == 0) {
      int plid, maxTime;
      if ((sscanf(req, "SNG %06d %03d\n", &plid, &maxTime) != 2) || (plid < 1 || plid > 999999 || maxTime < 1 || maxTime > 600)) {
        sprintf(_buf, "RSG ERR\n");
      } else if (_sessions->getPLIDSession(plid)->_playing) {
        sprintf(_buf, "RSG NOK\n");
      } else {
        _sessions->startSession(plid, maxTime);
        sprintf(_buf, "RSG OK\n");
      }

      return _buf;
    }

    if (strncmp(req, "TRY", 3) == 0) {
      int plid, nT, res;
      GameSession *game;
      char guess[8];
      memset(guess, ' ', sizeof(guess));
      guess[7] = '\0';

      if ((sscanf(req, "TRY %06d %c %c %c %c %d\n", &plid, &guess[0], &guess[2], &guess[4], &guess[6], &nT) != 6) || (plid < 1 || plid > 999999)) {
        sprintf(_buf, "RTR ERR\n");
      }
      /// horrible naming btw
      game = _sessions->getPLIDSession(plid);
      printf("game: %p\n", game);
      if (game == NULL || !game->_playing) {
        sprintf(_buf, "RTR NOK\n");
        return _buf;
      }

      if (nT < 1 || nT > 8) {
        sprintf(_buf, "RTR INV\n");
        return _buf;
      }

      Trial t = Trial(guess, 7);
      if (!t.isValid()){
        sprintf(_buf, "RTR ERR\n");
        return _buf;
      }



      res = _sessions->getPLIDSession(plid)->executeTrial(t, nT-1);

      switch (res){
        case GameSession::TrialResult::WIN:
          sprintf(_buf, "RTR OK %d %d %d %s\n", nT, 4, 0, guess);
          break;
        case GameSession::TrialResult::PLAYING:
        // was segfaulting here because nt was actually pid
          sprintf(_buf, "RTR OK %d %s %s\n", nT, game->_trials[nT-1].getnBnW(), guess);
          break;
        case GameSession::TrialResult::DUPLICATE:
          sprintf(_buf, "RTR DUP\n");
        case GameSession::TrialResult::INVALID:
          sprintf(_buf, "RTR INV\n");
          break;
        case GameSession::TrialResult::TIMEOUT:
          sprintf(_buf, "RTR ETM %s\n", game->_code.getTrial());
          break;
        case GameSession::TrialResult::LOSS:
          sprintf(_buf, "RTR ENT %d %s %s\n", nT, game->_trials[nT-1].getnBnW(), game->_code.getTrial());
          break;
      }
    }

    if (strncmp(req, "QUT", 3) == 0) {
      int plid;
      if ((sscanf(req, "QUT %06d\n", &plid) != 1) || (plid < 1 || plid > 999999) || _sessions->getPLIDSession(plid) == NULL || !_sessions->getPLIDSession(plid)->_playing) {
        sprintf(_buf, "RQT NOK\n");
      }
      sprintf(_buf, "RQT OK %s\n", _sessions->getPLIDSession(plid)->_code.getTrial());
    }

    if (strncmp(req, "DBG", 3) == 0) {
      int plid, maxTime;
      char key[8];
      memset(key, ' ', sizeof(key));
      key[7] = '\0';

      if ((sscanf(req, "DBG %06d %03d %c %c %c %c\n", &plid, &maxTime, &key[0], &key[2], &key[4], &key[6]) != 6) || (plid < 1 || plid > 999999 || maxTime < 1 || maxTime > 600) || !validCode(key)) {
        printf(key);
        sprintf(_buf, "RDB ERR\n");
        return _buf;
      }

      if (_sessions->getPLIDSession(plid)->_playing) {
        sprintf(_buf, "RDB NOK\n");
      } else {
        _sessions->startSession(plid, maxTime, key);
        sprintf(_buf, "RDB OK\n");
      }
      return _buf;

    }
    return _buf;
  }

};


#endif  // UDPSERVERPARSER_HPP_
