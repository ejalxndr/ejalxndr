#ifndef PORTFOLIO_CSS_H
#define PORTFOLIO_CSS_H

#include "portfolio/strbuf.h"

typedef struct css_prop_s {
  char *name;
  char *value;
  struct css_prop_s *next;
} css_prop_t;

typedef struct css_rule_s {
  char *selector;
  css_prop_t *props;
  struct css_rule_s *next;
} css_rule_t;

typedef struct css_media_s {
  char *condition;
  css_rule_t *rules;
  struct css_media_s *next;
} css_media_t;

typedef struct css_face_s {
  char *family;
  char *url;
  char *format;
  char *weight;
  char *style;
  char *display;
  struct css_face_s *next;
} css_face_t;

typedef struct {
  css_rule_t *rules;
  css_media_t *media;
  css_face_t *faces;
} css_stylesheet_t;

void css(css_stylesheet_t *sheet, const char *selector, const char *prop,
         const char *value);
void css_in(css_stylesheet_t *sheet, const char *condition,
            const char *selector, const char *prop, const char *value);
void css_font_face(css_stylesheet_t *sheet, const char *family, const char *url,
                   const char *format, const char *weight, const char *style,
                   const char *display);
void css_render(const css_stylesheet_t *sheet, strbuf_t *out);
void css_free(css_stylesheet_t *sheet);

#endif
