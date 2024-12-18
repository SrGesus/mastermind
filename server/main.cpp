#include <stdio.h>
#include <sys/select.h>

#include "common/utils.hpp"
#include "server/GameStorage.hpp"
#include "server/TCPServer.hpp"
#include "server/TCPServerParser.hpp"
#include "server/UDPServer.hpp"
#include "server/UDPServerParser.hpp"

const char *DEFAULT_IP = "0.0.0.0";
const char *DEFAULT_PORT = "58071";

int main(int argc, char **argv) {
  const char *ip = DEFAULT_IP;
  const char *port = DEFAULT_PORT;

  // Handle CLI Flags
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

  INFO("GSPort is %s\n", port);

  GameStorage gameStore = GameStorage();
  UDPServer udpServer = UDPServer(port, ip);
  TCPServer tcpServer = TCPServer(port, ip);
  UDPServerParser udpParser = UDPServerParser(gameStore);
  TCPServerParser tcpParser = TCPServerParser(gameStore);

  fd_set rfds, testfds;
  FD_ZERO(&rfds);                          // Clear input mask
  FD_SET(udpServer.socket().fd(), &rfds);  // Set UDP Channel on
  FD_SET(tcpServer.socket().fd(), &rfds);  // Set TCP Channel on
  int nfds = std::max(udpServer.socket().fd(), tcpServer.socket().fd());

  while (1) {
    testfds = rfds;
    int ready = select(nfds + 1, &testfds, nullptr, nullptr, nullptr);
    if (ready == -1) {
      WARN("Select failed: %s\n", strerror(errno));
    }
    if (FD_ISSET(udpServer.socket().fd(), &testfds)) {
      DEBUG("Processing UDP\n");
      udpServer.processRequest(udpParser);
    }
    if (FD_ISSET(tcpServer.socket().fd(), &testfds)) {
      DEBUG("Processing TCP\n");

      // Exit if process is child.
      if (tcpServer.processRequest(tcpParser)) return 0;
    }
  }

  return 0;
}