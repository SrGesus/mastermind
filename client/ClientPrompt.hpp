#ifndef CLIENTPROMPT_HPP_
#define CLIENTPROMPT_HPP_

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <client/TCPClient.hpp>
#include <client/UDPClient.hpp>
#include <common/Color.hpp>

#define CASE(X)                                                    \
  case CommandSum::X:                                              \
    status = strcmp(buffer, CommandStr::X) ? -2 : handle##X(args); \
    break;

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
    static constexpr const char *Help = "help";
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
    Help = csum(CommandStr::Help),
    UDP = csum(CommandStr::UDP),
    TCP = csum(CommandStr::TCP),
  };

  static const int PLID_SIZE = 6;

 private:
  UDPClient _udpClient;
  TCPClient _tcpClient;
  bool _playing = false;
  int _plid = 0;
  int _nT;

 public:
  ClientPrompt(const char *ip, const char *port)
      : _udpClient(ip, port), _tcpClient(ip, port) {}

  /* ------------------------------- Start -------------------------------- */
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
      DEBUG("INVALID PLID: %d\n\n", plid);
      printStartUsage();
      return -1;
    }

    if (maxTime < 1 || maxTime > 600) {
      DEBUG("INVALID max_playtime: %d\n\n", maxTime);
      printStartUsage();
      return -1;
    }

    snprintf(req, sizeof(req), "SNG %06d %03d\n", plid, maxTime);

    const char *resp = _udpClient.runCommand(req);

    if (strcmp(resp, "RSG OK\n") == 0) {
      printf("New game started (max %d sec).\n", maxTime);
      _playing = true;
      _nT = 1;
      _plid = plid;
      return 0;
    }

    if (strcmp(resp, "RSG NOK\n") == 0) {
      printf(
          "Game already in progress for this PLID, please wait until its "
          "end.\n");
      return -1;
    }

    printf("Could not start a new game, please try again...\n");
    return -1;
  }

  /* ------------------------------- Debug -------------------------------- */

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
      WARN("INVALID PLID: %d\n\n", plid);
      printDebugUsage();
      return -1;
    }

    if (maxTime < 1 || maxTime > 600) {
      WARN("INVALID max_playtime: %d\n\n", maxTime);
      printDebugUsage();
      return -1;
    }

    if (!validColor(c1) || !validColor(c2) || !validColor(c3) ||
        !validColor(c4)) {
      WARN("INVALID Colors: %c %c %c %c\n", c1, c2, c3, c4);
      printColors();
    }

    snprintf(req, sizeof(req), "DBG %06d %03d %c %c %c %c\n", plid, maxTime, c1,
             c2, c3, c4);

    const char *resp = _udpClient.runCommand(req);

    if (strcmp(resp, "RDB NOK\n") == 0) {
      printf(
          "Game already in progress for this PLID, please wait until its "
          "end.\n");
      return -1;
    }

    if (strcmp(resp, "RDB OK\n") == 0) {
      printf("New game started (max %d sec).\n", maxTime);
      _playing = true;
      _nT = 1;
      _plid = plid;
      return 0;
    }

    printf("Could not start a new game, please try again...\n");
    return -1;
  }

  /* ------------------------------- Try -------------------------------- */

  bool validColor(char c) {
    switch (c) {
      case Color::Red:
      case Color::Green:
      case Color::Blue:
      case Color::Yellow:
      case Color::Orange:
      case Color::Purple:
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
    printf("\t%c - Red\n", Color::Red);
    printf("\t%c - Green\n", Color::Green);
    printf("\t%c - Blue\n", Color::Blue);
    printf("\t%c - Yellow\n", Color::Yellow);
    printf("\t%c - Orange\n", Color::Orange);
    printf("\t%c - Purple\n", Color::Purple);
  }

  int handleTry(const char *args) {
    char req[32];
    char c1, c2, c3, c4, newLine;

    if (sscanf(args, "%c %c %c %c%c", &c1, &c2, &c3, &c4, &newLine) != 5 ||
        newLine != '\n') {
      printTryUsage();
      return -1;
    }

    // Convert lowercase characters to upper
    c1 = toupper(c1);
    c2 = toupper(c2);
    c3 = toupper(c3);
    c4 = toupper(c4);

    if (!validColor(c1) || !validColor(c2) || !validColor(c3) ||
        !validColor(c4)) {
      printTryUsage();
      return -1;
    }

    if (!_playing) {
      printf(
          "There is no game ongoing at the moment.\nPlease start a new "
          "game with \"start\".\n");
      return -1;
    }

    snprintf(req, sizeof(req), "TRY %06d %c %c %c %c %d\n", _plid, c1, c2, c3,
             c4, _nT);

    const char *resp = _udpClient.runCommand(req);

    int nT, nB, nW;
    if (sscanf(resp, "RTR OK %1d %1d %1d\n", &nT, &nB, &nW) == 3) {
      if (nB == 4) {
        printf("WELL DONE! You guessed the key in %d trials.\n", nT);
        _playing = false;
      } else {
        printf("nB = %d, nW = %d\n", nB, nW);
      }
      _nT++;
      return 0;
    }

    if (strcmp(resp, "RTR DUP\n") == 0) {
      printf(
          "Trial \"%c %c %c %c\" was already attempted.\nTry another guess.\n",
          c1, c2, c3, c4);
      return 0;
    }

    if (sscanf(resp, "RTR ENT %c %c %c %c\n", &c1, &c2, &c3, &c4) == 4) {
      printf(
          "GUESS LIMIT EXCEEDED! The correct key was \"%c %c %c %c\".\nBetter "
          "luck next time :(\n",
          c1, c2, c3, c4);
      _playing = false;
      return -1;
    }

    if (sscanf(resp, "RTR ETM %c %c %c %c\n", &c1, &c2, &c3, &c4) == 4) {
      printf(
          "TIME LIMIT EXCEEDED! The correct key was \"%c %c %c %c\".\nBetter "
          "luck next time :(\n",
          c1, c2, c3, c4);
      _playing = false;
      return -1;
    }

    if (strcmp(resp, "RTR INV\n") == 0 || strcmp(resp, "RTR NOK\n") == 0) {
      printf(
          "Client state is out of sync with server. Please start a new game\n");
      _playing = false;
      return -1;
    }

    printf("Could not handle attempt, please try again...\n");
    return -1;
  }

  /* --------------------------- Show Trials ------------------------------ */

  int handleSt(const char *args) { return handleShowTrials(args); }

  int handleShowTrials(const char *args) {
    char req[32];

    if (_plid == 0) {
      printf(
          "There is no game ongoing at the moment.\nPlease start a new "
          "game with \"start\".\n");
      return -1;
    }

    snprintf(req, sizeof(req), "STR %06d\n", _plid);

    const char *resp = _tcpClient.runCommand(req);

    char status[4], fname[32], fsize[32];
    int pos;
    if (sscanf(resp, "RST %3s %s %s %n", status, fname, fsize, &pos) == 3) {
      if (strcmp("FIN", status) == 0) {
        _playing = false;
      } else if (strcmp("ACT", status) != 0) {
        printf("Could not show trials for plid: \"%06d\"\n", _plid);
        return -1;
      }

      FILE *f;
      // Files on parent directories are not to be touched
      if (strstr(fname, "..") != nullptr ||
          (f = fopen(fname, "w")) == nullptr) {
        WARN("Failed to open file \"%s\" for saving: %s\n", fname,
             strerror(errno));
        return -1;
      }
      fprintf(f, "%s", resp + pos);
      fclose(f);
      fprintf(stdout, "%s", resp + pos);
      fprintf(stdout, "Trials were written to file: %s\n", fname);

      return 0;
    }

    printf("Could not show trials for plid: \"%06d\"\n", _plid);
    return -1;
  }

  /* --------------------------- Scoreboard ------------------------------ */
  int handleSb(const char *args) { return handleScoreboard(args); }

  int handleScoreboard(const char *args) {
    char req[32];

    snprintf(req, sizeof(req), "SSB\n");

    const char *resp = _tcpClient.runCommand(req);

    char fname[32], fsize[32];
    int pos;
    if (sscanf(resp, "RSS OK %s %s %n", fname, fsize, &pos) == 2) {
      FILE *f;
      // Files on parent directories are not to be touched
      if (strstr(fname, "..") != nullptr ||
          (f = fopen(fname, "w")) == nullptr) {
        WARN("Failed to open file \"%s\" for saving: %s\n", fname,
             strerror(errno));
        return -1;
      }
      fprintf(f, "%s", resp + pos);
      fclose(f);
      fprintf(stdout, "%s", resp + pos);
      fprintf(stdout, "Scoreboard was written to file: %s\n", fname);

      return 0;
    }

    printf("Could not show trials for plid: \"%06d\"\n", _plid);
    return -1;
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
      snprintf(req, sizeof(req), "QUT %06d\n", _plid);

      const char *resp = _udpClient.runCommand(req);
      (void)resp;

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

    int status;
    const char *args = buffer + e + 1;
    switch (checksum(buffer)) {
      CASE(Start)
      CASE(Try)
      CASE(ShowTrials)
      CASE(St)
      CASE(Scoreboard)
      CASE(Sb)
      CASE(Quit)
      CASE(Exit)
      CASE(Debug)
      CASE(UDP)
      CASE(TCP)
      default:  // Unknown command
        status = -2;
    }
    if (status == -2) WARN("Unrecognized command: %s\n", buffer);
    return status;
  }

  int handleUDP(const char *args) {
    const char *resp = _udpClient.runCommand(args);

    printf("%s", resp);
    return 0;
  }

  int handleTCP(const char *args) {
    const char *resp = _tcpClient.runCommand(args);

    printf("%s", resp);
    return 0;
  }
};

#undef CASE

#endif  // CLIENTPROMPT_HPP_