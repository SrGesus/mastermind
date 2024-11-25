#ifndef GAMESTORAGE_HPP
#define GAMESTORAGE_HPP

#include <unordered_map>
#include <vector>
#include <algorithm>
#include "server/GameSession.hpp"

class GameStorage {
    public:
        GameSession *getPLIDSession(int plid) {
            return &_sessions[plid];
        }

        void startSession(int plid, int maxTime) {
            _sessions.erase(plid);
            _sessions[plid] = GameSession(plid, maxTime);
        }

        void startSession(int plid, int maxTime, char code[8]) {
            _sessions.erase(plid);
            _sessions[plid] = GameSession(plid, maxTime, code);
        }

        std::vector<GameSession> getScoreboard() {
            std::vector<GameSession> scoreboard;
            for (auto const& [key, val] : _sessions) {
                scoreboard.push_back(val);
            }

            std::sort(scoreboard.begin(), scoreboard.end(), [](GameSession a, GameSession b) {
                return a._score > b._score;
            });
            
            if (scoreboard.size() > 10) {
                scoreboard.resize(10);
            }
            return scoreboard;
        }

    private:
        std::unordered_map<int, GameSession> _sessions;
        
};
#endif  // GAMESTORAGE_HPP_
