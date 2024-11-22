#ifndef TRIAL_HPP_
#define TRIAL_HPP_

#include <string.h>
#include <unistd.h>

enum Color : char {
  red = 'R',
  green = 'G',
  blue = 'B',
  yellow = 'Y',
  orange = 'O',
  purple = 'P'
};

class Trial {
 public:
  /// @brief Number of colors in one trial
  static const size_t NUMBER_COLORS = 4;
  /// @brief Size of the string that represents a Trial + Results
  static const size_t SIZE = (NUMBER_COLORS + 2) * 2;

 private:
  /// @brief C1 C2 C3 C4 [nB nW]
  char _trial[Trial::SIZE];

 public:
  /// @brief Creates trial
  /// @param in String representation of trial C1 C2 C3 C4 [nB nW]
  /// @param n Size of in, should be either 7 or 11.
  Trial(const char *in, size_t n) {
    strncpy(_trial, in, n);
    _trial[n] = '\0';
  }

  /// @return String representation of trial C1 C2 C3 C4 [nB nW]
  const char *getTrial() { return _trial; }
  /// @return String representation of results nB nW
  const char *getnBnW() { return _trial + NUMBER_COLORS * 2; }

  /// @brief Calculates nB and nW for Trial and adds it to the string.
  /// @param code Secret code.
  /// @return True if nB == 4
  bool evaluateNumbers(const Trial &code) {
    _trial[NUMBER_COLORS * 2 - 1] = ' ';
    char &nB = _trial[NUMBER_COLORS * 2] = '0';
    _trial[NUMBER_COLORS * 2 + 1] = ' ';
    char &nW = _trial[(NUMBER_COLORS + 1) * 2] = '0';
    for (size_t i = 0; i < NUMBER_COLORS; i++) {
      for (size_t j = 0; j < NUMBER_COLORS; j++) {
        bool isWhite = false;
        if (_trial[i * 2] == code._trial[j * 2]) {
          if (i == j) {
            nB++;
            break;
          } else {
            isWhite = true;
          }
        }
        nW += isWhite;
      }
    }
    return nB == '0' + NUMBER_COLORS;
  }

  /// @brief Check if trial is actually C1 C2 C3 C4, ignoring posterior
  /// characters.
  /// @return Whether trial is a valid combination of colors.
  bool isValid() {
    // Colors must be those listed
    for (size_t i = 0; i < NUMBER_COLORS; i++) {
      switch (_trial[i * 2]) {
        case Color::red:
        case Color::green:
        case Color::blue:
        case Color::yellow:
        case Color::orange:
        case Color::purple:
          break;
        default:  // Invalid color
          return false;
      }
    }
    // Colors should be separated by spaces
    for (size_t i = 0; i < NUMBER_COLORS - 1; i++) {
      if (_trial[i * 2 + 1] != ' ') {
        return false;
      }
    }
    return true;
  }
};

#endif  // TRIAL_HPP_
