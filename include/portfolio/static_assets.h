#ifndef PORTFOLIO_STATIC_ASSETS_H
#define PORTFOLIO_STATIC_ASSETS_H

#include <stddef.h>

typedef struct {
  const char *path;
  const char *file;
  const char *content_type;
} static_asset_t;

void static_assets_init(const char *dir);
const static_asset_t *static_assets_find(const char *path);
char *static_assets_read(const static_asset_t *asset, size_t *out_len);

#endif
