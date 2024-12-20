#ifndef GAMESESSION_HPP_
#define GAMESESSION_HPP_

#include <time.h>

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

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

  /// @brief Epoch time in seconds of game start.
  time_t _startTime = 0;
  /// @brief Provided time limit of game in seconds.
  uint32_t _maxTime : 10 = 0;
  /// @brief Time from start to end in seconds.
  uint32_t _duration : 10 = 0;
  uint32_t _nT : 4 = 1;
  uint32_t _debug : 1 = false;
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
    } else if (inProgress()) {  // Check if there is still time left
      // If not retry, trial must be next trial.
      if (nT != _nT) {
        return INVALID;
      }
      // Check if trial was not attempted before.
      for (const Trial &t : _trials) {
        if (trial == t) return DUPLICATE;
      }
      // Calculate numbers of blacks and whites
      bool victory = trial.evaluateNumbers(_code, nB, nW);
      // Register trial
      getTrial(_nT++) = trial;
      // Check if limit of trials was exceeded.
      if (_nT > TRIALS_NUMBER) {
        _lastResult = LOSS;
      }
      // Or if it was a victory
      if (victory) {
        _lastResult = WIN;
      }
    }
    // Cases: WIN/LOSS/PLAYING/TIMEOUT
    return _lastResult;
  }

  /// @brief Set game to no longer be in progress.
  /// @return Whether game was quited sucessfully or not if it wasn't in
  /// progress.
  int endGame() {
    if (inProgress()) {
      _lastResult = QUIT;
      return true;
    }
    return false;
  }

  /// @brief Get game score.
  int score() const { return _nT; }

  /// @brief Generates string representation of played trials.
  /// @return String representation of played trials.
  std::string showTrials(int plid) const {
    std::stringstream res;
    std::time_t result = std::time(nullptr);
    if (_lastResult == PLAYING) {
      res << "Ongoing";
    } else {
      res << "Finalized";
    }
    res << " game found for player " + std::to_string(plid) + "\n";
    res << std::put_time(std::localtime(&result), "%c %Z");
    res << "\n";
    if (_lastResult == WIN) {
      res << "Congratulations! You won in " + std::to_string(_nT) + " trials!\n";
    } else if (_lastResult == PLAYING) {
      res << "Currently playing trial " + std::to_string(_nT) + "!\n";
    } else {
      res << "You lost! The secret code was " + _code.toString() + "\n";
    }

    res << "\n" << " Trial " << " Code " << " nB nW" << "\n";
    uint16_t nB = 0, nW = 0;
    for (int i = TRIALS_NUMBER; i > 0; --i) {
      Trial t = getTrial(i);
      res << std::setw(6) << std::to_string(i) << "  " << t.toString() << "  ";
      if (i < _nT) {
        t.getnBW(nB, nW);
        res << std::setw(2) << std::to_string(nB) << " " << std::setw(2) << std::to_string(nW) << "\n";
      } else {
        res << "\n";
      }

    }
    res << std::setw(10) << " " << "\n";


    if (_lastResult == TIMEOUT) {
      res << "\nYou ran out of time!\n";
    } else {
      res << "\nYou have " + std::to_string(getRemaining()) + "s remaining!\n";
    }
    return res.str();
  }

  /// @return Remaining playing time in seconds.
  time_t getRemaining() const {
    return static_cast<time_t>(_maxTime - _duration);
  }

  /// @return Whether game is in progress. (Playing and not out of time)
  bool inProgress() {
    if (_lastResult == PLAYING) {
      _duration = std::clamp(time(NULL) - _startTime, static_cast<time_t>(0),
                             static_cast<time_t>(_maxTime));
      if (getRemaining() <= 0) _lastResult = TIMEOUT;
    }
    return _lastResult == PLAYING;
  }

  uint16_t nT() { return _nT; }

  bool exists() { return _lastResult != ERROR; }
};
static constexpr int e = sizeof(GameSession);

#endif  // GAMESESSION_HPP_