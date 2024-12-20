#ifndef GAMESTORAGE_HPP
#define GAMESTORAGE_HPP

#include <algorithm>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>

#include "server/GameSession.hpp"

class GameStorage {
 private:
  std::unordered_map<int, GameSession> _sessions;
  std::vector<std::pair<int, GameSession>> _scoreboard;

  // Delete copy constructor to prevent accidental copies
  GameStorage(const GameStorage&) = delete;
  GameStorage& operator=(const GameStorage&) = delete;

 public:
  GameStorage() {}

  GameSession& newSession(int plid, GameSession s) {
    return (_sessions[plid] = s);
  }

  GameSession& getSession(int plid) { return _sessions[plid]; }

  /// @brief Add a session to the scoreboard.
  /// @param plid Player ID associated with the session.
  /// @param s Session to be added.
  /// @note Remember, score means nT, lower is better!
  void addToScoreboard(int plid, GameSession s) {
    if (_scoreboard.size() < 10) {
      _scoreboard.push_back(std::make_pair(plid, s));
    } else {
      if (s.score() < _scoreboard.back().second.score()) {
        _scoreboard.back() = std::make_pair(plid, s);
      }
    }
    std::stable_sort(_scoreboard.begin(), _scoreboard.end(),
                     [](const std::pair<int, GameSession>& a,
                        const std::pair<int, GameSession>& b) {
                       return a.second.score() > b.second.score();
                     });
  }

  std::string getScoreboardString() {
    if (_scoreboard.empty()) return "";
    std::stringstream str;
    str << "+-----------------------------------------------------------+\n";
    str << "|                       TOP " << std::setw(2)
        << std::to_string(_scoreboard.size())
        << " SCORES                       |\n";
    str << "+----+-------+--------+------+-----------+-------+----------+\n"
           "|    | SCORE | PLAYER | CODE | NO TRIALS |  MODE | DURATION |\n";
    for (size_t i = 0; i < _scoreboard.size(); i++) {
      const auto& s = _scoreboard[i];
      str << "|" << std::setw(3) << std::to_string(i + 1) << " |  "
          << std::setw(3) << std::to_string(s.second.score()) << "  | "
          << std::setw(6) << std::to_string(s.first) << " | "
          << s.second.getCode().toString() << " |     "
          << std::to_string(s.second.nT()) << "     | "
          << (s.second.debug() ? "DEBUG" : " PLAY") << " |   " << std::setw(3)
          << std::to_string(s.second.duration()) << "s   |\n";
    }
    str << "+----+-------+--------+------+-----------+-------+----------+\n";
    return str.str();
  }
};
#endif  // GAMESTORAGE_HPP_
