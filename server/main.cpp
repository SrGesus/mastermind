#include <stdio.h>
#include <string.h>

#include "server/GameStorage.hpp"
#include "server/UDPServer.hpp"
#include "server/UDPServerParser.hpp"

const char *DEFAULT_IP = "localhost";
const char *DEFAULT_PORT = "58000";

int main(int argc, char **argv) {
  const char *ip = DEFAULT_IP;
  const char *port = DEFAULT_PORT;

  bool verbose_flag = false;
  ;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
      port = argv[++i];
    else if (strcmp(argv[i], "-v") == 0) {
      verbose_flag = true;
    } else {
      fprintf(stderr, "Usage: %s [-p port] [-v]\n", argv[0]);
      return 1;
    }
  }

  GameStorage sessions = GameStorage();
  UDPServer s = UDPServer(ip, port, verbose_flag);
  UDPServerParser parser = UDPServerParser(&sessions);

  while (1) {
    s.processRequest(parser);
  }

  return 0;
}