
#include <stdio.h>
#include <string.h>

#include <client/ClientPrompt.hpp>
#include <client/UDPClient.hpp>
#include <common/utils.hpp>

const char *DEFAULT_IP = "localhost";
const char *DEFAULT_PORT = "58000";

int main(int argc, char **argv) {
  const char *GSIp = DEFAULT_IP, *GSPort = DEFAULT_PORT;

  // Handle CLI Flags
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-n") == 0 && i + 1 < argc)
      GSIp = argv[++i];
    else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
      GSPort = argv[++i];
    else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0)
      utils_debug_flag = true;
    else {
      fprintf(stderr, "Usage: %s [-n ip] [-p port] [-d]\n", argv[0]);
      exit(1);
    }
  }

  DEBUG("GSPort is %s\n", GSPort);
  DEBUG("GSIp is %s\n", GSIp);

  ClientPrompt p = ClientPrompt(GSIp, GSPort);

  while (p.processCommand() != 1);

  exit(0);
}
