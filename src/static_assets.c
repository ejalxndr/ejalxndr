#define _POSIX_C_SOURCE 200809L

#include "portfolio/static_assets.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "portfolio/fileutil.h"

static const static_asset_t ASSETS[] = {
    {"/avatar.png", "img/avatar.png", "image/png"},
    {"/favicon-32.png", "img/favicon-32.png", "image/png"},
    {"/favicon-64.png", "img/favicon-64.png", "image/png"},
    {"/apple-touch-icon.png", "img/apple-touch-icon.png", "image/png"},
    {"/fonts/Geist-Variable.woff2", "fonts/Geist-Variable.woff2", "font/woff2"},
};
static const size_t NUM_ASSETS = sizeof(ASSETS) / sizeof(ASSETS[0]);

static char g_static_dir[PATH_MAX] = ".";

void static_assets_init(const char *dir) {
  snprintf(g_static_dir, sizeof g_static_dir, "%s", dir);
}

const static_asset_t *static_assets_find(const char *path) {
  for (size_t i = 0; i < NUM_ASSETS; i++)
    if (strcmp(path, ASSETS[i].path) == 0)
      return &ASSETS[i];
  return NULL;
}

char *static_assets_read(const static_asset_t *asset, size_t *out_len) {
  char full_path[PATH_MAX + 256];
  snprintf(full_path, sizeof full_path, "%s/%s", g_static_dir, asset->file);
  return file_read_all(full_path, out_len);
}
