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
  char _c[NUMBER_COLORS];
  char _nB, _nW;

 public:
  /// @brief Verifies trial colors are valid.
  /// @return Validity
  bool isValid() {
    // Colors must be those listed
    for (size_t i = 0; i < NUMBER_COLORS; i++) {
      switch (_c[i]) {
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
    return true;
  }

  /// @brief Creates trial
  /// @param in String representation of trial C1 C2 C3 C4 [nB nW]
  /// @param n Size of in, should be either 7 or 11.
  Trial(char c1, char c2, char c3, char c4) {
    _c[0] = c1;
    _c[1] = c2;
    _c[2] = c3;
    _c[3] = c4;
  }

  /// @brief Get color array.
  /// @return 4 character buffer where each character is the color of the ith
  /// guess.
  const char *getC() { return (char *)_c; }
  const int getnB() { return _nB; }
  const int getnW() { return _nW; }

  /// @brief Calculates nB and nW for Trial and adds it to the string.
  /// @param code Secret code.
  /// @return True if nB == 4
  bool evaluateNumbers(const Trial &code) {
    _nB = 0, _nW = 0;
    for (size_t i = 0; i < NUMBER_COLORS; i++) {
      bool isWhite = false, isBlack = false;
      for (size_t j = 0; j < NUMBER_COLORS; j++) {
        if (_c[i] == code._c[j]) {
          if (i == j) {
            isBlack = true;
            break;
          } else {
            isWhite = true;
          }
        }
      }
      _nW += isWhite && !isBlack;
      _nB += isBlack;
    }
    return _nB == NUMBER_COLORS;
  }
};

#endif  // TRIAL_HPP_
