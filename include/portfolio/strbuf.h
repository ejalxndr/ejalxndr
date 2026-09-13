#ifndef PORTFOLIO_STRBUF_H
#define PORTFOLIO_STRBUF_H

#include <stddef.h>

typedef struct {
  char *data;
  size_t len;
  size_t cap;
} strbuf_t;

void strbuf_init(strbuf_t *sb);
void strbuf_reserve(strbuf_t *sb, size_t extra);
void strbuf_putsn(strbuf_t *sb, const char *s, size_t n);
void strbuf_puts(strbuf_t *sb, const char *s);
void strbuf_appendf(strbuf_t *sb, const char *fmt, ...);
void strbuf_free(strbuf_t *sb);

#endif
