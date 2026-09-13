#ifndef PORTFOLIO_HTML_H
#define PORTFOLIO_HTML_H

#include "portfolio/strbuf.h"

typedef enum {
  HTML_NODE_ELEMENT,
  HTML_NODE_TEXT,
  HTML_NODE_RAW,
} html_node_kind_t;

typedef struct html_attr_s {
  char *name;
  char *value;
  struct html_attr_s *next;
} html_attr_t;

typedef struct html_node_s {
  html_node_kind_t kind;
  char *tag;
  html_attr_t *attrs;
  struct html_node_s **children;
  size_t n_children;
  size_t cap_children;
  char *text;
} html_node_t;

html_node_t *html_el(const char *tag);
html_node_t *html_text(const char *s);
html_node_t *html_raw(const char *s);
html_node_t *html_el_text(const char *tag, const char *txt);
html_node_t *html_append(html_node_t *parent, html_node_t *child);
void html_attr(html_node_t *n, const char *name, const char *value);
void html_render(const html_node_t *root, strbuf_t *out);
void html_free(html_node_t *n);

#endif
