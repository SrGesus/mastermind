
#include <stdio.h>
#include <string.h>

#include <client/ClientPrompt.hpp>
#include <client/UDPClient.hpp>
#include <common/utils.hpp>

const char *DEFAULT_IP = "localhost";
const char *DEFAULT_PORT = "58000";

int main(int argc, char **argv) {
  const char *GSIp = DEFAULT_IP, *GSPort = DEFAULT_PORT;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) GSIp = argv[i + 1];
    if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) GSPort = argv[i + 1];
    if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0)
      utils_debug_flag = true;
  }

  DEBUG("GSPort is %s\n", GSPort);
  DEBUG("GSIp is %s\n", GSIp);

  ClientPrompt p = ClientPrompt(GSIp, GSPort);

  while (p.prompt() != 1);

  exit(0);
}
