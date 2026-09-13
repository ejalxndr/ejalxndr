#include "portfolio/page.h"

#include <stdio.h>

#include "portfolio/css.h"
#include "portfolio/html.h"
#include "portfolio/strbuf.h"
#include "portfolio/visits.h"

static void build_stylesheet(css_stylesheet_t *sheet) {
  css_font_face(sheet, "Geist", "./fonts/Geist-Variable.woff2",
                "woff2-variations", "100 900", "normal", "swap");

  css(sheet, ":root[data-theme=\"dark\"]", "color-scheme", "dark");
  css(sheet, ":root[data-theme=\"dark\"]", "--bg", "#0a0a0a");
  css(sheet, ":root[data-theme=\"dark\"]", "--fg", "#ededed");
  css(sheet, ":root[data-theme=\"dark\"]", "--muted", "#a1a1aa");

  css(sheet, ":root[data-theme=\"light\"]", "color-scheme", "light");
  css(sheet, ":root[data-theme=\"light\"]", "--bg", "#ffffff");
  css(sheet, ":root[data-theme=\"light\"]", "--fg", "#0a0a0a");
  css(sheet, ":root[data-theme=\"light\"]", "--muted", "#71717a");

  css(sheet, "*", "box-sizing", "border-box");

  css(sheet, "body", "margin", "0");
  css(sheet, "body", "min-height", "100vh");
  css(sheet, "body", "background", "var(--bg)");
  css(sheet, "body", "color", "var(--fg)");
  css(sheet, "body", "font-family",
      "\"Geist\", ui-sans-serif, system-ui, -apple-system, \"Segoe UI\", "
      "Roboto, Helvetica, Arial, sans-serif");
  css(sheet, "body", "-webkit-font-smoothing", "antialiased");
  css(sheet, "body", "transition",
      "background-color 0.15s ease, color 0.15s ease");

  css(sheet, "main", "max-width", "42rem");
  css(sheet, "main", "margin", "0 auto");
  css(sheet, "main", "padding", "4rem 1.5rem 6rem");

  css_in(sheet, "(min-width: 768px)", "main", "padding", "4rem 2rem 6rem");

  css(sheet, "nav", "display", "flex");
  css(sheet, "nav", "justify-content", "flex-end");
  css(sheet, "nav", "margin-bottom", "3rem");

  css(sheet, "#theme-toggle", "background", "none");
  css(sheet, "#theme-toggle", "border", "none");
  css(sheet, "#theme-toggle", "padding", "0");
  css(sheet, "#theme-toggle", "font", "inherit");
  css(sheet, "#theme-toggle", "font-size", "0.875rem");
  css(sheet, "#theme-toggle", "color", "var(--muted)");
  css(sheet, "#theme-toggle", "cursor", "pointer");
  css(sheet, "#theme-toggle", "transition", "color 0.15s ease");

  css(sheet, "#theme-toggle:hover", "color", "var(--fg)");

  css(sheet, ".avatar", "width", "5rem");
  css(sheet, ".avatar", "height", "5rem");
  css(sheet, ".avatar", "object-fit", "cover");
  css(sheet, ".avatar", "filter", "grayscale(1)");
  css(sheet, ".avatar", "display", "block");
  css(sheet, ".avatar", "margin-bottom", "1.5rem");

  css(sheet, "h1", "font-size", "1.875rem");
  css(sheet, "h1", "font-weight", "600");
  css(sheet, "h1", "letter-spacing", "-0.05em");
  css(sheet, "h1", "margin", "0 0 1.5rem 0");

  css_in(sheet, "(min-width: 768px)", "h1", "font-size", "2.25rem");

  css(sheet, "p", "font-size", "1.125rem");
  css(sheet, "p", "line-height", "1.75rem");
  css(sheet, "p", "color", "var(--muted)");
  css(sheet, "p", "margin", "0 0 1rem 0");

  css(sheet, "p:last-of-type", "margin-bottom", "0");

  css(sheet, "a", "color", "white");
  css(sheet, "a", "font-weight", "600");

  css(sheet, ".links", "display", "flex");
  css(sheet, ".links", "flex-wrap", "wrap");
  css(sheet, ".links", "gap", "1.25rem");
  css(sheet, ".links", "padding-top", "2rem");
  css(sheet, ".links", "font-size", "0.875rem");
  css(sheet, ".links", "color", "var(--muted)");

  css(sheet, ".links a", "color", "inherit");
  css(sheet, ".links a", "text-decoration", "none");
  css(sheet, ".links a", "transition", "color 0.15s ease");

  css(sheet, ".links a:hover", "color", "var(--fg)");
}

static html_node_t *build_head(const char *style_css) {
  html_node_t *head = html_el("head");

  html_node_t *meta_charset = html_append(head, html_el("meta"));
  html_attr(meta_charset, "charset", "UTF-8");

  html_node_t *meta_viewport = html_append(head, html_el("meta"));
  html_attr(meta_viewport, "name", "viewport");
  html_attr(meta_viewport, "content", "width=device-width, initial-scale=1.0");

  html_node_t *meta_desc = html_append(head, html_el("meta"));
  html_attr(meta_desc, "name", "description");
  html_attr(meta_desc, "content", "Ethan Alexander");

  html_append(head, html_el_text("title", "Ethan Alexander"));

  html_node_t *icon32 = html_append(head, html_el("link"));
  html_attr(icon32, "rel", "icon");
  html_attr(icon32, "type", "image/png");
  html_attr(icon32, "sizes", "32x32");
  html_attr(icon32, "href", "./favicon-32.png");

  html_node_t *icon64 = html_append(head, html_el("link"));
  html_attr(icon64, "rel", "icon");
  html_attr(icon64, "type", "image/png");
  html_attr(icon64, "sizes", "64x64");
  html_attr(icon64, "href", "./favicon-64.png");

  html_node_t *touch_icon = html_append(head, html_el("link"));
  html_attr(touch_icon, "rel", "apple-touch-icon");
  html_attr(touch_icon, "sizes", "180x180");
  html_attr(touch_icon, "href", "./apple-touch-icon.png");

  html_node_t *theme_script = html_append(head, html_el("script"));
  html_append(
      theme_script,
      html_raw(
          "\n"
          "        document.documentElement.dataset.theme = "
          "localStorage.getItem('theme') === 'light' ? 'light' : 'dark'\n"));

  html_node_t *style_el = html_append(head, html_el("style"));
  html_append(style_el, html_raw(style_css));

  return head;
}

static html_node_t *build_links(unsigned long long visits) {
  html_node_t *links = html_el("div");
  html_attr(links, "class", "links");

  html_node_t *gh_link = html_append(links, html_el("a"));
  html_attr(gh_link, "href", "https://github.com/shdwmtr");
  html_attr(gh_link, "rel", "noreferrer");
  html_attr(gh_link, "target", "_blank");
  html_append(gh_link, html_text("GitHub"));

  html_node_t *email_link = html_append(links, html_el("a"));
  html_attr(email_link, "href", "mailto:contact@ethanjalexander.ca");
  html_append(email_link, html_text("Email"));

  char count_str[32];
  visits_format(visits, count_str, sizeof count_str);
  char visits_text[64];
  snprintf(visits_text, sizeof visits_text, "%s visits", count_str);
  html_append(links, html_el_text("span", visits_text));

  return links;
}

static html_node_t *build_main(unsigned long long visits) {
  html_node_t *main_el = html_el("main");

  html_node_t *nav = html_append(main_el, html_el("nav"));
  html_node_t *toggle = html_append(nav, html_el("button"));
  html_attr(toggle, "id", "theme-toggle");
  html_attr(toggle, "type", "button");

  html_node_t *avatar = html_append(main_el, html_el("img"));
  html_attr(avatar, "class", "avatar");
  html_attr(avatar, "src", "./avatar.png");
  html_attr(avatar, "alt", "Ethan Alexander");

  html_append(main_el, html_el_text("h1", "Hi. I'm Ethan."));

  html_node_t *p1 = html_append(main_el, html_el("p"));
  html_append(p1, html_text("I am a full time student majoring in Computer "
                            "Science. I specialize in building, "
                            "maintaining, and optimizing high-throughput "
                            "infrastructure where poor decisions "
                            "carry real-time consequences."));

  html_node_t *p2 = html_append(main_el, html_el("p"));
  html_append(p2, html_text("I'm a massive "));
  html_node_t *foss_link = html_append(p2, html_el("a"));
  html_attr(foss_link, "href",
            "https://en.wikipedia.org/wiki/Free_and_open-source_software");
  html_append(foss_link, html_text("FOSS"));
  html_append(
      p2, html_text(" enthusiast. Most of my current open-source work is on "));
  html_node_t *mill_link = html_append(p2, html_el("a"));
  html_attr(mill_link, "href",
            "https://github.com/SteamClientHomebrew/Millennium");
  html_append(mill_link, html_text("Millennium"));
  html_append(p2, html_text(", which I solely design and develop. Millennium "
                            "has a constantly growing "
                            "user-base of over 5M+ people."));

  html_append(main_el, build_links(visits));

  return main_el;
}

static html_node_t *build_body(unsigned long long visits) {
  html_node_t *body = html_el("body");
  html_append(body, build_main(visits));

  html_node_t *toggle_script = html_append(body, html_el("script"));
  html_append(
      toggle_script,
      html_raw("\n"
               "        var toggle = document.getElementById('theme-toggle')\n"
               "\n"
               "        function render() {\n"
               "            toggle.textContent = "
               "document.documentElement.dataset.theme === 'dark' ? 'Light' : "
               "'Dark'\n"
               "        }\n"
               "\n"
               "        toggle.addEventListener('click', function () {\n"
               "            var next = document.documentElement.dataset.theme "
               "=== 'dark' ? 'light' : 'dark'\n"
               "            document.documentElement.dataset.theme = next\n"
               "            localStorage.setItem('theme', next)\n"
               "            render()\n"
               "        })\n"
               "\n"
               "        render()\n"));

  return body;
}

char *page_render_index(unsigned long long visits, size_t *out_len) {
  css_stylesheet_t sheet = {0};
  build_stylesheet(&sheet);

  strbuf_t css_out;
  strbuf_init(&css_out);
  css_render(&sheet, &css_out);
  css_free(&sheet);

  html_node_t *html = html_el("html");
  html_attr(html, "lang", "en");
  html_attr(html, "data-theme", "dark");

  html_append(html, build_head(css_out.data));
  strbuf_free(&css_out);

  html_append(html, build_body(visits));

  strbuf_t out;
  strbuf_init(&out);
  strbuf_puts(&out, "<!DOCTYPE html>\n");
  html_render(html, &out);

  html_free(html);

  if (out_len)
    *out_len = out.len;
  return out.data;
}
