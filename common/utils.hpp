#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <stdio.h>

/// @todo Revisit this value
const int BUFFER_SIZE = 128;

/// @brief This macro is for fatal errors that cannot be recovered from.
#define ERROR(...)                            \
  {                                           \
    fprintf(stderr, "[Error]: " __VA_ARGS__); \
    exit(1);                                  \
  }

/// @brief This macro is for anomalies that might be safely ignored.
#define WARN(...) fprintf(stderr, "[Warn]: " __VA_ARGS__)

#endif  // UTILS_HPP_
