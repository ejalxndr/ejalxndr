#include "portfolio/server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "portfolio/http.h"
#include "portfolio/page.h"
#include "portfolio/static_assets.h"
#include "portfolio/visits.h"

#define REQUEST_BUF_SIZE 8192

static void serve_index(int fd, unsigned long long visits, int is_head) {
  size_t len;
  char *body = page_render_index(visits, &len);
  if (!body) {
    http_send_plain(fd, 500, "Internal Server Error", is_head);
    return;
  }
  http_send_response(fd, 200, "OK", "text/html; charset=utf-8", body, len,
                     is_head);
  free(body);
}

static void serve_static(int fd, const static_asset_t *asset, int is_head) {
  size_t len;
  char *data = static_assets_read(asset, &len);
  if (!data) {
    http_send_plain(fd, 404, "Not Found", is_head);
    return;
  }
  http_send_response(fd, 200, "OK", asset->content_type, data, len, is_head);
  free(data);
}

static void route(int fd, const http_request_t *req, int is_head) {
  if (strcmp(req->path, "/") == 0 || strcmp(req->path, "/index.html") == 0) {
    unsigned long long v = is_head ? visits_current() : visits_bump();
    serve_index(fd, v, is_head);
    return;
  }

  const static_asset_t *asset = static_assets_find(req->path);
  if (asset) {
    serve_static(fd, asset, is_head);
    return;
  }

  http_send_plain(fd, 404, "Not Found", is_head);
}

static void *handle_connection(void *arg) {
  int fd = (int)(intptr_t)arg;

  struct timeval tv = {.tv_sec = 5, .tv_usec = 0};
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
  setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv);

  char buf[REQUEST_BUF_SIZE];
  ssize_t n = http_read_request(fd, buf, sizeof buf);
  if (n <= 0) {
    close(fd);
    return NULL;
  }

  http_request_t req;
  if (http_parse_request_line(buf, &req) != 0) {
    http_send_plain(fd, 400, "Bad Request", 0);
    close(fd);
    return NULL;
  }

  int is_head = strcmp(req.method, "HEAD") == 0;
  int is_get = strcmp(req.method, "GET") == 0;

  if (!is_get && !is_head) {
    http_send_plain(fd, 405, "Method Not Allowed", is_head);
    close(fd);
    return NULL;
  }

  route(fd, &req, is_head);

  close(fd);
  return NULL;
}

int server_run(const server_config_t *config) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(config->bind_addr);
  addr.sin_port = htons((uint16_t)config->port);

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof addr) < 0) {
    perror("bind");
    return 1;
  }
  if (listen(server_fd, 128) < 0) {
    perror("listen");
    return 1;
  }

  fprintf(stdout, "listening on %s:%d\n", config->bind_addr, config->port);
  fflush(stdout);

  for (;;) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof client_addr;
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
      if (errno == EINTR)
        continue;
      perror("accept");
      continue;
    }

    pthread_t tid;
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&tid, &thread_attr, handle_connection,
                       (void *)(intptr_t)client_fd) != 0) {
      close(client_fd);
    }
    pthread_attr_destroy(&thread_attr);
  }

  return 0;
}
