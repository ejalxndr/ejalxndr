#ifndef PORTFOLIO_SERVER_H
#define PORTFOLIO_SERVER_H

typedef struct {
  const char *bind_addr;
  int port;
} server_config_t;

int server_run(const server_config_t *config);

#endif
