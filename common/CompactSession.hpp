#ifndef COMPACTSESSION_HPP_
#define COMPACTSESSION_HPP_

#include <stdint.h>

enum ColorChar {
  Red = 'R',
  Green = 'G',
  Blue = 'B',
  Yellow = 'Y',
  Orange = 'O',
  Purple = 'P'
};

#define COLOR_CASE(X)      \
  case (char)ColorChar::X: \
    return ColorByte::X;

#define CHAR_CASE(X) \
  case ColorByte::X: \
    return ColorChar::X;

class Trial {
  enum ColorByte : char { None, Red, Green, Blue, Yellow, Orange, Purple };

  ColorByte _c1 : 4;
  ColorByte _c2 : 4;
  ColorByte _c3 : 4;
  ColorByte _c4 : 4;

 public:
  Trial() {
    _c1 = ColorByte::None;
    _c2 = ColorByte::None;
    _c3 = ColorByte::None;
    _c4 = ColorByte::None;
  }

  Trial(char c1, char c2, char c3, char c4) {
    _c1 = char_to_color(c1);
    _c2 = char_to_color(c2);
    _c3 = char_to_color(c3);
    _c4 = char_to_color(c4);
  }

  bool operator==(Trial t) {
    return _c1 == t._c1 && _c2 == t._c2 && _c3 == t._c3 && _c4 == t._c4;
  }

  char c1() { return byte_to_color(_c1); }
  char c2() { return byte_to_color(_c2); }
  char c3() { return byte_to_color(_c3); }
  char c4() { return byte_to_color(_c4); }

  bool is_valid() { return _c1 != 0 && _c2 != 0 && _c3 != 0 && _c4 != 0; }

  static ColorByte char_to_color(char c) {
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
  }

  static char byte_to_color(uint16_t c) {
    switch (c) {
      CHAR_CASE(Red)
      CHAR_CASE(Green)
      CHAR_CASE(Blue)
      CHAR_CASE(Yellow)
      CHAR_CASE(Orange)
      CHAR_CASE(Purple)
      default:
        return 0;
    }
  }
};

#undef CHAR_CASE
#undef COLOR_CASE

#endif  // COMPACTSESSION_HPP_