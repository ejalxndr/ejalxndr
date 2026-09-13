#define _POSIX_C_SOURCE 200809L

#include "portfolio/html.h"

#include <stdlib.h>
#include <string.h>

static const char *VOID_TAGS[] = {
    "area",  "base", "br",   "col",   "embed",  "hr",    "img",
    "input", "link", "meta", "param", "source", "track", "wbr",
};

static int is_void_tag(const char *tag) {
  for (size_t i = 0; i < sizeof(VOID_TAGS) / sizeof(VOID_TAGS[0]); i++)
    if (strcmp(tag, VOID_TAGS[i]) == 0)
      return 1;
  return 0;
}

static html_node_t *node_new(html_node_kind_t kind) {
  html_node_t *n = calloc(1, sizeof(html_node_t));
  n->kind = kind;
  return n;
}

html_node_t *html_el(const char *tag) {
  html_node_t *n = node_new(HTML_NODE_ELEMENT);
  n->tag = strdup(tag);
  return n;
}

html_node_t *html_text(const char *s) {
  html_node_t *n = node_new(HTML_NODE_TEXT);
  n->text = strdup(s);
  return n;
}

html_node_t *html_raw(const char *s) {
  html_node_t *n = node_new(HTML_NODE_RAW);
  n->text = strdup(s);
  return n;
}

html_node_t *html_append(html_node_t *parent, html_node_t *child) {
  if (parent->n_children == parent->cap_children) {
    parent->cap_children = parent->cap_children ? parent->cap_children * 2 : 4;
    parent->children =
        realloc(parent->children, parent->cap_children * sizeof(html_node_t *));
  }
  parent->children[parent->n_children++] = child;
  return child;
}

void html_attr(html_node_t *n, const char *name, const char *value) {
  html_attr_t *a = malloc(sizeof(html_attr_t));
  a->name = strdup(name);
  a->value = strdup(value);
  a->next = NULL;
  if (!n->attrs) {
    n->attrs = a;
    return;
  }
  html_attr_t *cur = n->attrs;
  while (cur->next)
    cur = cur->next;
  cur->next = a;
}

html_node_t *html_el_text(const char *tag, const char *txt) {
  html_node_t *n = html_el(tag);
  html_append(n, html_text(txt));
  return n;
}

static void put_text_escaped(strbuf_t *sb, const char *s) {
  for (const char *p = s; *p; p++) {
    switch (*p) {
    case '&':
      strbuf_puts(sb, "&amp;");
      break;
    case '<':
      strbuf_puts(sb, "&lt;");
      break;
    case '>':
      strbuf_puts(sb, "&gt;");
      break;
    default:
      strbuf_putsn(sb, p, 1);
    }
  }
}

static void put_attr_escaped(strbuf_t *sb, const char *s) {
  for (const char *p = s; *p; p++) {
    switch (*p) {
    case '&':
      strbuf_puts(sb, "&amp;");
      break;
    case '"':
      strbuf_puts(sb, "&quot;");
      break;
    default:
      strbuf_putsn(sb, p, 1);
    }
  }
}

static void put_indent(strbuf_t *sb, int depth) {
  for (int i = 0; i < depth; i++)
    strbuf_puts(sb, "    ");
}

static void render_node(const html_node_t *n, strbuf_t *sb, int depth) {
  if (n->kind == HTML_NODE_TEXT) {
    put_indent(sb, depth);
    put_text_escaped(sb, n->text);
    strbuf_puts(sb, "\n");
    return;
  }
  if (n->kind == HTML_NODE_RAW) {
    strbuf_puts(sb, n->text);
    return;
  }

  put_indent(sb, depth);
  strbuf_appendf(sb, "<%s", n->tag);
  for (html_attr_t *a = n->attrs; a; a = a->next) {
    strbuf_appendf(sb, " %s=\"", a->name);
    put_attr_escaped(sb, a->value);
    strbuf_puts(sb, "\"");
  }

  if (is_void_tag(n->tag)) {
    strbuf_puts(sb, ">\n");
    return;
  }

  if (n->n_children == 0) {
    strbuf_appendf(sb, "></%s>\n", n->tag);
    return;
  }

  if (n->n_children == 1 && n->children[0]->kind == HTML_NODE_TEXT) {
    strbuf_puts(sb, ">");
    put_text_escaped(sb, n->children[0]->text);
    strbuf_appendf(sb, "</%s>\n", n->tag);
    return;
  }

  strbuf_puts(sb, ">\n");
  for (size_t i = 0; i < n->n_children; i++)
    render_node(n->children[i], sb, depth + 1);
  put_indent(sb, depth);
  strbuf_appendf(sb, "</%s>\n", n->tag);
}

void html_render(const html_node_t *root, strbuf_t *out) {
  render_node(root, out, 0);
}

void html_free(html_node_t *n) {
  if (!n)
    return;
  html_attr_t *a = n->attrs;
  while (a) {
    html_attr_t *next = a->next;
    free(a->name);
    free(a->value);
    free(a);
    a = next;
  }
  for (size_t i = 0; i < n->n_children; i++)
    html_free(n->children[i]);
  free(n->children);
  free(n->tag);
  free(n->text);
  free(n);
}
