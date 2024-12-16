#include <stdio.h>
#include <string.h>

#include "common/utils.hpp"
#include "server/GameStorage.hpp"
#include "server/UDPServer.hpp"
#include "server/UDPServerParser.hpp"

const char *DEFAULT_IP = "localhost";
const char *DEFAULT_PORT = "58000";

int main(int argc, char **argv) {
  const char *ip = DEFAULT_IP;
  const char *port = DEFAULT_PORT;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
      port = argv[++i];
    else if (strcmp(argv[i], "-v") == 0) {
      utils_verbose_flag = true;
    } else if (strcmp(argv[i], "-d") == 0) {
      utils_debug_flag = true;
    } else {
      fprintf(stderr, "Usage: %s [-p port] [-v] [-d]\n", argv[0]);
      return 1;
    }
  }

  INFO("GSIp is %s\n", ip);
  INFO("GSPort is %s\n", port);

  GameStorage gameStore = GameStorage();
  UDPServer s = UDPServer(ip, port);
  UDPServerParser parser = UDPServerParser(gameStore);

  while (1) {
    s.processRequest(parser);
  }

  return 0;
}