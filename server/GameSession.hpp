#ifndef GAMESESSION_HPP_
#define GAMESESSION_HPP_

#include <time.h>

#include <cstdlib>

#include "Trial.hpp"

/// @brief Class that represents a Game Session in 32 bytes.
class GameSession {
 public:
  /// @brief Result of a trial.
  enum TrialResult : uint8_t {
    // ERR
    ERROR,
    // NOK
    QUIT,
    // DUP
    DUPLICATE,
    // INV
    INVALID,
    // ETM
    TIMEOUT,
    // ENT
    LOSS,
    // OK
    WIN,
    // OK
    PLAYING
  };

 private:
  /// @brief Maximum number of guesses that can be made in a game.
  static constexpr int TRIALS_NUMBER = 8;

  time_t _startTime = 0;
  uint16_t _maxTime : 10 = 0;
  uint16_t _remainingTime : 10 = 0;
  uint16_t _nT : 4 = 1;
  uint8_t _debug : 1 = false;
  uint8_t _score : 7 = 0;
  TrialResult _lastResult = ERROR;
  Trial _code;
  Trial _trials[TRIALS_NUMBER];

  /// @brief Getter for trial
  /// @param nT Trial number
  /// @return Trial
  Trial &getTrial(int nT) { return _trials[nT - 1]; }

 public:
  /// @brief Default constructor. Resets seed on random number generator.
  GameSession() {}

  /// @brief Method that creates a session with a given secret code.
  /// @param maxTime Session time limit.
  /// @param code Secret Code to be used.
  /// @return New Session.
  static GameSession newDebugGame(int maxTime, Trial code) {
    GameSession session;
    session._startTime = time(NULL);
    session._maxTime = maxTime;
    session._debug = true;
    session._lastResult = PLAYING;
    session._code = code;
    return session;
  }

  /// @brief Method that creates a session with a random secret code.
  /// @param maxTime Session time limit.
  /// @return New Session.
  static GameSession newGame(int maxTime) {
    GameSession session = newDebugGame(maxTime, Trial::random());
    session._debug = false;
    return session;
  }

  /// @brief Getter for trial
  /// @param nT Trial number
  /// @return Trial
  const Trial &getTrial(int nT) const { return _trials[nT - 1]; }

  const Trial &getCode() const { return _code; }

  /// @brief Generates string representation of played trials.
  /// @return String representation of played trials.
  std::string showTrials() const {
    std::string s;
    for (int i = 1; i < _nT; i++) {
      s += getTrial(i).toString() + "\n";
    }
    return s + std::to_string(_remainingTime) + "\n";
  }

  /// @brief Attempts to execute a trial
  /// @param trial To be executed.
  /// @param nT Integer between 1 and 8 representing the trial number.
  /// @param nB Reference where number of Blacks will be written.
  /// @param nW Reference where number of Whites will be written.
  /// @note nB and nW is only written if WIN/LOSS/PLAYING is returned.
  /// @return Result of the attempt.
  TrialResult executeTrial(Trial &trial, int nT, uint16_t &nB, uint16_t &nW) {
    // Check is trial number is within bounds and trial is well formed.
    if (nT < 1 || nT > TRIALS_NUMBER || !trial.isValid()) {
      return ERROR;
    }

    DEBUG(
        "Attemping trial %c %c %c %c, nT=%d, _lastResult=%d, Secret (%c %c %c "
        "%c)\n",
        trial.c1(), trial.c2(), trial.c3(), trial.c4(), nT, _lastResult,
        _code.c1(), _code.c2(), _code.c3(), _code.c4());

    // Check if trial is a retry.
    if (nT == _nT - 1 && trial == getTrial(nT)) {
      getTrial(_nT - 1).getnBW(nB, nW);
      return _lastResult;
    }

    // If game has already ended, handling should be a bit different.
    if (_lastResult != PLAYING) {
      if (_lastResult == LOSS || _lastResult == WIN) {
        // If it is not a retry, it is out of context.
        return QUIT;
      }
      // If there was a timeout and it would not be the next trial,
      // it also is out of context.
      if (_lastResult == TIMEOUT && nT != _nT) {
        return QUIT;
      }
      // Cases: TIMEOUT/QUIT/ERROR
      return _lastResult;
    }

    // Check if there is still time left
    if (!checkTime()) {
      _lastResult = TIMEOUT;
      return _lastResult;
    }
    // Otherwise trial must be next trial.
    if (nT != _nT) {
      return INVALID;
    }
    // Check if trial was not attempted before.
    for (const Trial &t : _trials) {
      if (trial == t) return DUPLICATE;
    }

    getTrial(_nT++) = trial;
    // Calculate numbers of blacks and whites
    bool victory = trial.evaluateNumbers(_code, nB, nW);
    // Check if limit of trials was exceeded.
    if (_nT == TRIALS_NUMBER) {
      _lastResult = LOSS;
    }
    // Or if it was a victory
    if (victory) {
      _lastResult = WIN;
    }
    return _lastResult;
  }

  /// @brief Set game to no longer be in progress.
  void endGame() { 
    _lastResult = QUIT;
    updateRemaining(time(nullptr));
  }

  /// @brief Get game score.
  int score() const { return _nT; }

  /// @brief Updates the remaining time.
  /// @param currentTime 
  /// @return Updated remaining time for session.
  uint16_t updateRemaining(time_t currentTime) {
    uint16_t duration = currentTime - _startTime;
    if (duration > _maxTime) { 
      _remainingTime = 0;
    } else {
      _remainingTime = _maxTime - duration;
    }
    return _remainingTime;
  }
  /// @brief If time has expired will end game.
  /// @return Whether game is within time limits.
  bool checkTime() {
    time_t current = time(nullptr);
    updateRemaining(current);
    if (current - _startTime > _maxTime) {
      _lastResult = TIMEOUT;
      return false;
    } else {
      return true;
    }
  }

  /// @return Whether game is in progress. (Playing and not out of time)
  bool inProgress() { return _lastResult == PLAYING && checkTime(); }

  uint16_t nT() { return _nT; }
  TrialResult lastResult() { return _lastResult; }

  bool exists() { return _lastResult != ERROR; }
};
static constexpr int e = sizeof(GameSession);

#endif  // GAMESESSION_HPP_