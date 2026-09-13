#include "portfolio/http.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define RESPONSE_HEADER_BUF 512

static ssize_t write_all(int fd, const void *buf, size_t len) {
  const char *p = buf;
  size_t remaining = len;
  while (remaining > 0) {
    ssize_t n = write(fd, p, remaining);
    if (n < 0) {
      if (errno == EINTR)
        continue;
      return -1;
    }
    p += n;
    remaining -= (size_t)n;
  }
  return (ssize_t)len;
}

ssize_t http_read_request(int fd, char *buf, size_t bufsize) {
  size_t total = 0;
  while (total < bufsize - 1) {
    ssize_t n = read(fd, buf + total, bufsize - 1 - total);
    if (n < 0) {
      if (errno == EINTR)
        continue;
      return -1;
    }
    if (n == 0)
      break;
    total += (size_t)n;
    buf[total] = '\0';
    if (strstr(buf, "\r\n\r\n"))
      break;
  }
  return (ssize_t)total;
}

int http_parse_request_line(const char *buf, http_request_t *req) {
  if (sscanf(buf, "%15s %1023s", req->method, req->path) != 2)
    return -1;
  char *query = strchr(req->path, '?');
  if (query)
    *query = '\0';
  return 0;
}

void http_send_response(int fd, int status, const char *status_text,
                        const char *content_type, const void *body,
                        size_t body_len, int is_head) {
  char header[RESPONSE_HEADER_BUF];
  int hlen = snprintf(header, sizeof header,
                      "HTTP/1.1 %d %s\r\n"
                      "Content-Type: %s\r\n"
                      "Content-Length: %zu\r\n"
                      "Connection: close\r\n"
                      "\r\n",
                      status, status_text, content_type, body_len);
  if (hlen < 0)
    return;
  write_all(fd, header, (size_t)hlen);
  if (!is_head && body_len > 0)
    write_all(fd, body, body_len);
}

void http_send_plain(int fd, int status, const char *status_text, int is_head) {
  char body[64];
  int n = snprintf(body, sizeof body, "%d %s", status, status_text);
  http_send_response(fd, status, status_text, "text/plain", body, (size_t)n,
                     is_head);
}
