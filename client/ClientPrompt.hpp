#ifndef CLIENTPROMPT_HPP_
#define CLIENTPROMPT_HPP_

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <client/UDPClient.hpp>
#include <common/Trial.hpp>

#define CASE(X)       \
  case CommandSum::X: \
    return strcmp(buffer, CommandStr::X) ? -1 : handle##X(args);

static constexpr unsigned int csum(const char *str) {
  return str[0] + (str[0] != '\0' ? csum(str + 1) : 0);
}

int checksum(const char *str) {
  return str[0] + (str[0] != '\0' ? checksum(str + 1) : 0);
}

class ClientPrompt {
  class CommandStr {
   public:
    static constexpr const char *Start = "start";
    static constexpr const char *Try = "try";
    static constexpr const char *ShowTrials = "show_trials";
    static constexpr const char *St = "st";
    static constexpr const char *Scoreboard = "scoreboard";
    static constexpr const char *Sb = "sb";
    static constexpr const char *Quit = "quit";
    static constexpr const char *Exit = "exit";
    static constexpr const char *Debug = "debug";
    static constexpr const char *UDP = "udp";
    static constexpr const char *TCP = "tcp";
  };

  enum CommandSum : unsigned int {
    Start = csum(CommandStr::Start),
    Try = csum(CommandStr::Try),
    ShowTrials = csum(CommandStr::ShowTrials),
    St = csum(CommandStr::St),
    Scoreboard = csum(CommandStr::Scoreboard),
    Sb = csum(CommandStr::Sb),
    Quit = csum(CommandStr::Quit),
    Exit = csum(CommandStr::Exit),
    Debug = csum(CommandStr::Debug),
    UDP = csum(CommandStr::UDP),
    TCP = csum(CommandStr::TCP),
  };

  static const int PLID_SIZE = 6;

 private:
  UDPClient _udpClient;
  bool _playing = false;
  int _plid;
  int _nT;

 public:
  ClientPrompt(const char *ip, const char *port) : _udpClient(ip, port) {}

  void printStartUsage() {
    printf(
        "Invalid syntax for \"start\" command.\n\n"
        "Usage: start PLID max_playtime\n"
        "\tPLID - 6 digit Player Identification number.\n"
        "\tmax_playtime - time limit in seconds, must not be greater than "
        "600.\n");
  }

  int handleStart(const char *args) {
    char req[32];

    int plid, maxTime;
    char space, newLine;
    if (sscanf(args, "%6d%c%3d%c", &plid, &space, &maxTime, &newLine) != 4 ||
        space != ' ' || newLine != '\n') {
      printStartUsage();
      return -1;
    }

    if (plid < 1 || plid > 999999) {
      DEBUG("INVALID PLID: %d\n", plid);
      printStartUsage();
      return -1;
    }

    if (maxTime < 1 || maxTime > 600) {
      DEBUG("INVALID max_playtime: %d\n", maxTime);
      printStartUsage();
      return -1;
    }

    sprintf(req, "SNG %06d %03d\n", plid, maxTime);

    const char *resp = _udpClient.runCommand(req);

    // TODO: Handle responses
    _playing = true;
    _nT = 1;
    _plid = plid;

    return 0;
  }

  bool validColor(char c) {
    switch (c) {
      case Color::red:
      case Color::green:
      case Color::blue:
      case Color::yellow:
      case Color::orange:
      case Color::purple:
        return true;
      default:  // Invalid color
        return false;
    }
  }

  void printTryUsage() {
    printf(
        "Invalid syntax for \"try\" command.\n\n"
        "Usage: try C1 C2 C3 C4\n"
        "\tCi - Color of the ith guess.\n");
    printColors();
  }

  void printColors() {
    printf("\nColors:\n");
    printf("\t%c - Red\n", Color::red);
    printf("\t%c - Green\n", Color::green);
    printf("\t%c - Blue\n", Color::blue);
    printf("\t%c - Yellow\n", Color::yellow);
    printf("\t%c - Orange\n", Color::orange);
    printf("\t%c - Purple\n", Color::purple);
  }

  int handleTry(const char *args) {
    char req[32];
    char c1, c2, c3, c4, newLine;

    if (sscanf(args, "%c %c %c %c%c", &c1, &c2, &c3, &c4, &newLine) != 5 ||
        newLine != '\n' || !validColor(c1) || !validColor(c2) ||
        !validColor(c3) || !validColor(c4)) {
      printTryUsage();
      return -1;
    }

    if (!_playing) {
      printf(
          "There is no game ongoing at the moment.\nPlease start a new "
          "game with \"start\".\n");
    }

    sprintf(req, "TRY %06d %c %c %c %c %d\n", _plid, c1, c2, c3, c4, _nT);

    const char *resp = _udpClient.runCommand(req);

    // TODO: Handle responses
    _nT++;

    return 0;
  }

  void printDebugUsage() {
    printf(
        "Invalid syntax for \"debug\" command.\n\n"
        "Usage: debug PLID max_playtime C1 C2 C3 C4\n"
        "\tPLID - 6 digit Player Identification number.\n"
        "\tmax_playtime - time limit in seconds, must not be greater than "
        "600.\n"
        "\tCi - Color of the ith guess for the secret key.\n");
    printColors();
  }

  int handleDebug(const char *args) {
    char req[32];

    int plid, maxTime;
    char space, newLine, c1, c2, c3, c4;
    if (sscanf(args, "%6d%c%3d %c %c %c %c%c", &plid, &space, &maxTime, &c1,
               &c2, &c3, &c4, &newLine) != 8 ||
        space != ' ' || newLine != '\n') {
      printDebugUsage();
      return -1;
    }

    if (plid < 1 || plid > 999999) {
      DEBUG("INVALID PLID: %d\n", plid);
      printDebugUsage();
      return -1;
    }

    if (maxTime < 1 || maxTime > 600) {
      DEBUG("INVALID max_playtime: %d\n", maxTime);
      printDebugUsage();
      return -1;
    }

    sprintf(req, "DBG %06d %03d %c %c %c %c\n", plid, maxTime, c1, c2, c3, c4);

    const char *resp = _udpClient.runCommand(req);

    // TODO: Handle responses
    _playing = true;
    _nT = 1;
    _plid = plid;

    return 0;
  }

  int handleExit(const char *args) {
    quit();
    return 1;
  }

  int handleQuit(const char *args) {
    quit();
    return 0;
  }

  void quit() {
    if (_playing) {
      char req[50];
      sprintf(req, "QUT %06d\n", _plid);

      const char *resp = _udpClient.runCommand(req);

      char c1, c2, c3, c4;
      if (sscanf(resp, "RQT OK %c %c %c %c\n", &c1, &c2, &c3, &c4) == 4) {
        // TODO: End Game Message
        _playing = false;
      } else if (strcmp(resp, "RQT NOK\n")) {  // Game might already have ended.
        _playing = false;
      }
    }
  }

  /// @brief Prompts player for command.
  /// @return 1 if exit was called, 0 otherwise, -1 if there was an error.
  int prompt() {
    char buffer[BUFFER_SIZE];
    buffer[sizeof(buffer) - 1] = '\0';

    printf("> ");
    // Read stdin line to buffer.
    if (fgets(buffer, sizeof(buffer) - 1, stdin) == nullptr) {
      // EOF does the same as "exit" command
      clearerr(stdin);
      printf("\n");
      return handleExit(nullptr);
    }

    // Get first command before whitespace or newline
    int e = strcspn(buffer, " \n");
    buffer[e] = '\0';

    const char *args = buffer + e + 1;
    switch (checksum(buffer)) {
      CASE(Start)
      CASE(Try)
      // CASE(ShowTrials)
      // CASE(St)
      // CASE(Scoreboard)
      // CASE(Sb)
      CASE(Quit)
      CASE(Exit)
      CASE(Debug)
      CASE(UDP)
      // CASE(TCP)
      default:  // Unknown command
        WARN("Unrecognized command: %s\n", buffer);
        return -1;
    }
  }

  int handleUDP(const char *args) {
    const char *resp = _udpClient.runCommand(args);

    printf("%s", resp);
    return 0;
  }
};

#endif  // CLIENTPROMPT_HPP_