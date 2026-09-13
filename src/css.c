#define _POSIX_C_SOURCE 200809L

#include "portfolio/css.h"

#include <stdlib.h>
#include <string.h>

static css_rule_t *find_or_add_rule(css_rule_t **head, const char *selector) {
  css_rule_t *cur = *head, *prev = NULL;
  while (cur) {
    if (strcmp(cur->selector, selector) == 0)
      return cur;
    prev = cur;
    cur = cur->next;
  }
  css_rule_t *r = calloc(1, sizeof(css_rule_t));
  r->selector = strdup(selector);
  if (prev)
    prev->next = r;
  else
    *head = r;
  return r;
}

static void rule_add_prop(css_rule_t *r, const char *name, const char *value) {
  css_prop_t *p = malloc(sizeof(css_prop_t));
  p->name = strdup(name);
  p->value = strdup(value);
  p->next = NULL;
  if (!r->props) {
    r->props = p;
    return;
  }
  css_prop_t *cur = r->props;
  while (cur->next)
    cur = cur->next;
  cur->next = p;
}

void css(css_stylesheet_t *sheet, const char *selector, const char *prop,
         const char *value) {
  css_rule_t *r = find_or_add_rule(&sheet->rules, selector);
  rule_add_prop(r, prop, value);
}

static css_media_t *find_or_add_media(css_media_t **head,
                                      const char *condition) {
  css_media_t *cur = *head, *prev = NULL;
  while (cur) {
    if (strcmp(cur->condition, condition) == 0)
      return cur;
    prev = cur;
    cur = cur->next;
  }
  css_media_t *m = calloc(1, sizeof(css_media_t));
  m->condition = strdup(condition);
  if (prev)
    prev->next = m;
  else
    *head = m;
  return m;
}

void css_in(css_stylesheet_t *sheet, const char *condition,
            const char *selector, const char *prop, const char *value) {
  css_media_t *m = find_or_add_media(&sheet->media, condition);
  css_rule_t *r = find_or_add_rule(&m->rules, selector);
  rule_add_prop(r, prop, value);
}

void css_font_face(css_stylesheet_t *sheet, const char *family, const char *url,
                   const char *format, const char *weight, const char *style,
                   const char *display) {
  css_face_t *f = calloc(1, sizeof(css_face_t));
  f->family = strdup(family);
  f->url = strdup(url);
  f->format = strdup(format);
  f->weight = strdup(weight);
  f->style = strdup(style);
  f->display = strdup(display);
  if (!sheet->faces) {
    sheet->faces = f;
  } else {
    css_face_t *cur = sheet->faces;
    while (cur->next)
      cur = cur->next;
    cur->next = f;
  }
}

static void render_rule(const css_rule_t *r, strbuf_t *sb, int indent) {
  for (int i = 0; i < indent; i++)
    strbuf_puts(sb, "    ");
  strbuf_appendf(sb, "%s {\n", r->selector);
  for (css_prop_t *p = r->props; p; p = p->next) {
    for (int i = 0; i < indent + 1; i++)
      strbuf_puts(sb, "    ");
    strbuf_appendf(sb, "%s: %s;\n", p->name, p->value);
  }
  for (int i = 0; i < indent; i++)
    strbuf_puts(sb, "    ");
  strbuf_puts(sb, "}\n\n");
}

void css_render(const css_stylesheet_t *sheet, strbuf_t *out) {
  for (css_face_t *f = sheet->faces; f; f = f->next) {
    strbuf_puts(out, "@font-face {\n");
    strbuf_appendf(out, "    font-family: \"%s\";\n", f->family);
    strbuf_appendf(out, "    src: url(\"%s\") format(\"%s\");\n", f->url,
                   f->format);
    strbuf_appendf(out, "    font-weight: %s;\n", f->weight);
    strbuf_appendf(out, "    font-style: %s;\n", f->style);
    strbuf_appendf(out, "    font-display: %s;\n", f->display);
    strbuf_puts(out, "}\n\n");
  }
  for (css_rule_t *r = sheet->rules; r; r = r->next)
    render_rule(r, out, 0);
  for (css_media_t *m = sheet->media; m; m = m->next) {
    strbuf_appendf(out, "@media %s {\n", m->condition);
    for (css_rule_t *r = m->rules; r; r = r->next)
      render_rule(r, out, 1);
    strbuf_puts(out, "}\n\n");
  }
}

static void free_prop_list(css_prop_t *p) {
  while (p) {
    css_prop_t *next = p->next;
    free(p->name);
    free(p->value);
    free(p);
    p = next;
  }
}

static void free_rule_list(css_rule_t *r) {
  while (r) {
    css_rule_t *next = r->next;
    free_prop_list(r->props);
    free(r->selector);
    free(r);
    r = next;
  }
}

void css_free(css_stylesheet_t *sheet) {
  free_rule_list(sheet->rules);
  css_media_t *m = sheet->media;
  while (m) {
    css_media_t *next = m->next;
    free_rule_list(m->rules);
    free(m->condition);
    free(m);
    m = next;
  }
  css_face_t *f = sheet->faces;
  while (f) {
    css_face_t *next = f->next;
    free(f->family);
    free(f->url);
    free(f->format);
    free(f->weight);
    free(f->style);
    free(f->display);
    free(f);
    f = next;
  }
}
