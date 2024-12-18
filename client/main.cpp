
#include <stdio.h>
#include <string.h>

#include <client/ClientPrompt.hpp>
#include <client/UDPClient.hpp>
#include <common/utils.hpp>

const char *DEFAULT_IP = "localhost";
const char *DEFAULT_PORT = "58071";

int main(int argc, char **argv) {
  const char *ip = DEFAULT_IP, *port = DEFAULT_PORT;

  // Handle CLI Flags
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-n") == 0 && i + 1 < argc)
      ip = argv[++i];
    else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
      port = argv[++i];
    else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0)
      utils_debug_flag = true;
    else {
      fprintf(stderr, "Usage: %s [-n GSip] [-p GSport] [-d]\n", argv[0]);
      exit(1);
    }
  }

  DEBUG("GSPort is %s\n", port);
  DEBUG("GSIp is %s\n", ip);

  ClientPrompt p = ClientPrompt(ip, port);

  while (p.processCommand() != 1);

  exit(0);
}
