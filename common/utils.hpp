#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <stdio.h>
#include <time.h>

/// @todo Revisit this value
const int BUFFER_SIZE = 128;

bool utils_debug_flag = false;
bool utils_verbose_flag = false;

const char* ERR_RESPONSE = "ERR\n";

/// @brief This macro is for fatal errors that cannot be recovered from.
#define ERROR(...)                            \
  {                                           \
    fprintf(stderr, "[Error]: " __VA_ARGS__); \
    exit(1);                                  \
  }

/// @brief This macro is for anomalies that might be safely ignored.
#define WARN(...) fprintf(stderr, "[Warn]: " __VA_ARGS__)

/// @brief This macro is for verbose information.
#define DEBUG(...)                                     \
  if (utils_debug_flag) {                              \
    time_t my_time = time(NULL);                       \
    char _buf_[24];                                    \
    struct tm* tm_info = localtime(&my_time);          \
    strftime(_buf_, 24, "%Y-%m-%dT%H:%M:%S", tm_info); \
    fprintf(stdout, "[%s]: ", _buf_);                  \
    fprintf(stdout, "Debug - " __VA_ARGS__);           \
  }

#define VERBOSE(...)                                   \
  if (utils_verbose_flag) {                            \
    time_t my_time = time(NULL);                       \
    char _buf_[24];                                    \
    struct tm* tm_info = localtime(&my_time);          \
    strftime(_buf_, 24, "%Y-%m-%dT%H:%M:%S", tm_info); \
    fprintf(stdout, "[%s]: ", _buf_);                  \
    fprintf(stdout, "Verbose - " __VA_ARGS__);         \
  };

#define VERBOSE_APPEND(...)       \
  if (utils_verbose_flag) {       \
    fprintf(stdout, __VA_ARGS__); \
  };

#define INFO(...)                                      \
  {                                                    \
    time_t my_time = time(NULL);                       \
    char _buf_[24];                                    \
    struct tm* tm_info = localtime(&my_time);          \
    strftime(_buf_, 24, "%Y-%m-%dT%H:%M:%S", tm_info); \
    fprintf(stdout, "[%s]", _buf_);                    \
    fprintf(stdout, " Info:  " __VA_ARGS__);           \
  }

#endif  // UTILS_HPP_
