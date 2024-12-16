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

  void addToScoreboard(GameSession s) {}

  std::vector<GameSession> getScoreboard() {
    std::vector<GameSession> scoreboard;
    for (auto const& [key, val] : _sessions) {
      scoreboard.push_back(val);
    }

    std::sort(
        scoreboard.begin(), scoreboard.end(),
        [](GameSession a, GameSession b) { return a.score() > b.score(); });

    if (scoreboard.size() > 10) {
      scoreboard.resize(10);
    }
    return scoreboard;
  }

 private:
  std::unordered_map<int, GameSession> _sessions;
};
#endif  // GAMESTORAGE_HPP_
