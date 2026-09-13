#include "portfolio/strbuf.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void strbuf_init(strbuf_t *sb) {
  sb->cap = 64;
  sb->data = malloc(sb->cap);
  sb->data[0] = '\0';
  sb->len = 0;
}

void strbuf_reserve(strbuf_t *sb, size_t extra) {
  if (sb->len + extra + 1 <= sb->cap)
    return;
  size_t ncap = sb->cap ? sb->cap : 64;
  while (ncap < sb->len + extra + 1)
    ncap *= 2;
  sb->data = realloc(sb->data, ncap);
  sb->cap = ncap;
}

void strbuf_putsn(strbuf_t *sb, const char *s, size_t n) {
  strbuf_reserve(sb, n);
  memcpy(sb->data + sb->len, s, n);
  sb->len += n;
  sb->data[sb->len] = '\0';
}

void strbuf_puts(strbuf_t *sb, const char *s) {
  strbuf_putsn(sb, s, strlen(s));
}

void strbuf_appendf(strbuf_t *sb, const char *fmt, ...) {
  va_list ap, ap2;
  va_start(ap, fmt);
  va_copy(ap2, ap);
  int n = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);
  if (n < 0) {
    va_end(ap2);
    return;
  }
  strbuf_reserve(sb, (size_t)n);
  vsnprintf(sb->data + sb->len, (size_t)n + 1, fmt, ap2);
  va_end(ap2);
  sb->len += (size_t)n;
}

void strbuf_free(strbuf_t *sb) {
  free(sb->data);
  sb->data = NULL;
  sb->len = sb->cap = 0;
}
