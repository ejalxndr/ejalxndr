#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "portfolio/server.h"
#include "portfolio/static_assets.h"
#include "portfolio/visits.h"

#define PORT_DEFAULT 3005

int main(int argc, char **argv) {
  int port = PORT_DEFAULT;
  const char *bind_addr = "127.0.0.1";
  const char *static_dir = "assets";
  const char *counter_path = "data/visits.count";

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
      port = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--bind") == 0 && i + 1 < argc) {
      bind_addr = argv[++i];
    } else if (strcmp(argv[i], "--static-dir") == 0 && i + 1 < argc) {
      static_dir = argv[++i];
    } else if (strcmp(argv[i], "--counter-file") == 0 && i + 1 < argc) {
      counter_path = argv[++i];
    }
  }

  signal(SIGPIPE, SIG_IGN);

  static_assets_init(static_dir);
  visits_init(counter_path);

  server_config_t config = {.bind_addr = bind_addr, .port = port};
  return server_run(&config);
}
