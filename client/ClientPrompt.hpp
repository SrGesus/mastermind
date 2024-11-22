#ifndef CLIENTPROMPT_HPP_
#define CLIENTPROMPT_HPP_

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <client/UDPClient.hpp>

#define CASE(X)                                                    \
  case CommandSum::X:                                              \
    status = strcmp(buffer, CommandStr::X) ? -1 : handle##X(args); \
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

 private:
  UDPClient _udpClient;
  bool playing = false;
  static const int PLID_SIZE = 6;
  char PLID[PLID_SIZE];

 public:
  ClientPrompt(const char *ip, const char *port) : _udpClient(ip, port) {}

  void printStartUsage() {
    printf(
        "\nInvalid syntax for \"start\" command.\n\n"
        "Usage: start PLID max_playtime\n"
        "\tPLID - 6 digit Player Identification number.\n"
        "\tmax_playtime - time limit in seconds, must not be greater than "
        "600.\n");
  }

  int handleStart(const char *args) {
    char req[20];

    quit();

    strcpy(req, "SNG ");

    for (int i = 0; i < PLID_SIZE; i++) {
      if (!isdigit(args[i])) {
        DEBUG("INVALID PLID: %c is not a digit\n", args[i]);
        printStartUsage();
        return 0;
      }
      PLID[i] = args[i];
      (req + 4)[i] = args[i];
    }
    if (args[PLID_SIZE] != ' ') {
      printStartUsage();
      return 0;
    }

    char *pEnd;
    int maxTime = strtol(args + PLID_SIZE + 1, &pEnd, 10);
    if (maxTime > 600 || maxTime < 1 || pEnd - (args + PLID_SIZE + 1) > 3 ||
        pEnd[0] != '\n') {
      DEBUG("INVALID max_playtime: %s", args + PLID_SIZE + 1);
      printStartUsage();
      return 0;
    }
    strncpy(req + 4 + PLID_SIZE, args + PLID_SIZE, 6);

    playing = true;

    DEBUG("Sending via UDP: %s", req);

    const char *resp = _udpClient.runCommand(req);

    DEBUG("Received via UDP: %s", resp);

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
    char req[50];

    if (playing) {
      memcpy(req, "QUT ", 4);
      memcpy(req + 4, PLID, PLID_SIZE);
      req[4 + PLID_SIZE] = '\n';
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

    int status = 0;
    const char *args = buffer + e + 1;
    switch (checksum(buffer)) {
      CASE(Start)
      // CASE(Try)
      // CASE(ShowTrials)
      // CASE(St)
      // CASE(Scoreboard)
      // CASE(Sb)
      CASE(Quit)
      CASE(Exit)
      // CASE(Debug)
      CASE(UDP)
      // CASE(TCP)
      default:  // Unknown command
        status = -1;
        break;
    }
    if (status == -1) WARN("Unrecognized command: %s\n", buffer);
    return status;
  }

  int handleUDP(const char *args) {
    DEBUG("Sending via UDP: %s", args);

    const char *resp = _udpClient.runCommand(args);

    printf("%s", resp);
    return 0;
  }
};

#endif  // CLIENTPROMPT_HPP_