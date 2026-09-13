#ifndef PORTFOLIO_HTTP_H
#define PORTFOLIO_HTTP_H

#include <stddef.h>
#include <sys/types.h>

typedef struct {
  char method[16];
  char path[1024];
} http_request_t;

ssize_t http_read_request(int fd, char *buf, size_t bufsize);
int http_parse_request_line(const char *buf, http_request_t *req);
void http_send_response(int fd, int status, const char *status_text,
                        const char *content_type, const void *body,
                        size_t body_len, int is_head);
void http_send_plain(int fd, int status, const char *status_text, int is_head);

#endif
