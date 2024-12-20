#ifndef TRIAL_HPP_
#define TRIAL_HPP_

#include <cstdint>
#include <string>

#include "common/Color.hpp"
#include "common/utils.hpp"

/// @brief Class that represents a 4 Color code and their number of Black and
/// White in 2 bytes.
class Trial {
 private:
  enum ColorByte { None, Red, Green, Blue, Yellow, Orange, Purple };

  // Color stored in 3 bits each
  uint16_t _c1 : 3 = 0;
  uint16_t _c2 : 3 = 0;
  uint16_t _c3 : 3 = 0;
  uint16_t _c4 : 3 = 0;
  /// @brief Compact format for nB and nW
  uint16_t _nBW : 4 = 0;

  // Return colorbyte correspondent to each color.
  char bc1() const { return _c1; }
  char bc2() const { return _c2; }
  char bc3() const { return _c3; }
  char bc4() const { return _c4; }

 public:
  /// @brief Default Constructor creates an invalid trial.
  Trial() {}

  /// @brief Constructor from characters corresponding to the colors.
  Trial(char c1, char c2, char c3, char c4) {
    _c1 = color_to_byte(c1);
    _c2 = color_to_byte(c2);
    _c3 = color_to_byte(c3);
    _c4 = color_to_byte(c4);
    _nBW = 0;
  }

  /// @brief Generate a random trial using rand().
  static Trial random() {
    Trial t;
    t._c1 = (rand() % 6) + 1;
    t._c2 = (rand() % 6) + 1;
    t._c3 = (rand() % 6) + 1;
    t._c4 = (rand() % 6) + 1;
    return t;
  }

  bool operator==(const Trial &t) const {
    return _c1 == t._c1 && _c2 == t._c2 && _c3 == t._c3 && _c4 == t._c4;
  }

  bool operator!=(const Trial &t) const { return !operator==(t); }

  /// @brief Verifies trial colors are valid.
  /// @return Validity
  bool isValid() const { return _c1 != 0 && _c2 != 0 && _c3 != 0 && _c4 != 0; }

  // Return character correspondent to each color.
  char c1() const { return byte_to_color(_c1); }
  char c2() const { return byte_to_color(_c2); }
  char c3() const { return byte_to_color(_c3); }
  char c4() const { return byte_to_color(_c4); }

  /// @brief Save number of blacks and whites to given pointers.
  void getnBW(uint16_t &nBlack, uint16_t &nWhite) const {
    uint16_t nB = (_nBW & 0b1100) >> 2;
    uint16_t nW = _nBW & 0b0011;

    if (nB == 3 && nW > 1) {
      nBlack = nW == 3 ? 4 : 0;
      nWhite = nW == 3 ? 0 : 4;
    } else {
      nBlack = nB;
      nWhite = nW;
    }
  }

  /// @brief Sets nB and nW for trial
  /// @param nBlack Number of Black guesses
  /// @param nWhite Number of White guesses
  void setnBW(uint16_t nBlack, uint16_t nWhite) {
    if (nBlack == 4) {
      _nBW = 0b1110;
    } else if (nWhite == 4) {
      _nBW = 0b1111;
    } else {
      _nBW = (nBlack << 2) | nWhite;
    }
  }

  /// @brief Calculates nB and nW for the Trial.
  /// @param code Secret code.
  /// @return True if nB == 4
  bool evaluateNumbers(const Trial &code, uint16_t &nB, uint16_t &nW) {
    nB = 0, nW = 0;

    char codeCol[4] = {code.bc1(), code.bc2(), code.bc3(), code.bc4()};
    char colors[4] = {bc1(), bc2(), bc3(), bc4()};
    // Blacks
    for (int i = 0; i < 4; i++) {
      if (codeCol[i] == colors[i]) {
        // Don't compare this spot again
        colors[i] = ColorByte::None;
        codeCol[i] = ColorByte::None;
        nB++;
        continue;
      }
    }
    // Whites
    for (int i = 0; i < 4; i++) {
      if (codeCol[i] == 0) continue;
      for (int j = 0; j < 4; j++) {
        if (codeCol[i] == colors[j]) {
          // Don't compare this spot again
          colors[j] = ColorByte::None;
          nW++;
          break;
        }
      }
    }
    DEBUG("nB=%d nW=%d\n", nB, nW);
    setnBW(nB, nW);
    return nB == 4;
  }

  /// @brief Get the trial in a string format.
  std::string toString() const {
    return std::string(1, c1()) + c2() + c3() + c4();
  }

 private:
  static ColorByte color_to_byte(char c) {
#define COLOR_CASE(X)  \
  case (char)Color::X: \
    return ColorByte::X;

    switch (c) {
      COLOR_CASE(Red)
      COLOR_CASE(Green)
      COLOR_CASE(Blue)
      COLOR_CASE(Yellow)
      COLOR_CASE(Orange)
      COLOR_CASE(Purple)
      default:
        return ColorByte::None;
    }
#undef COLOR_CASE
  }

  static char byte_to_color(uint16_t c) {
#define CHAR_CASE(X) \
  case ColorByte::X: \
    return Color::X;

    switch (c) {
      CHAR_CASE(Red)
      CHAR_CASE(Green)
      CHAR_CASE(Blue)
      CHAR_CASE(Yellow)
      CHAR_CASE(Orange)
      CHAR_CASE(Purple)
      default:
        return '.';
    }
#undef CHAR_CASE
  }
};

#endif  // TRIAL_HPP_
