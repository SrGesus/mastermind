#ifndef GAMESTORAGE_HPP
#define GAMESTORAGE_HPP

#include <algorithm>
#include <optional>
#include <unordered_map>
#include <vector>

#include "server/GameSession.hpp"

class GameStorage {
 public:
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
             [](const std::pair<int, GameSession>& a, const std::pair<int, GameSession>& b) {
               return a.second.score() < b.second.score();
             });
  }


  std::vector<std::pair<int, GameSession>> getScoreboard() {
    return _scoreboard;
  }

  std::string getScoreboardString() {
    std::string scoreboardString;
    auto scoreboard = getScoreboard();
    for (auto const& s : scoreboard) {
      scoreboardString += std::to_string(s.first) + " " + s.second.getCode().toString() + " " + std::to_string(s.second.score()) + "\n";
    }
    return scoreboardString;
  }
 private:
  std::unordered_map<int, GameSession> _sessions;
  std::vector<std::pair<int, GameSession>> _scoreboard;
};
#endif  // GAMESTORAGE_HPP_
