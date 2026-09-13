#define _POSIX_C_SOURCE 200809L

#include "portfolio/visits.h"

#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "portfolio/fileutil.h"

static char *g_counter_path = NULL;
static unsigned long long g_visits = 0;
static pthread_mutex_t g_visits_lock = PTHREAD_MUTEX_INITIALIZER;

static unsigned long long load_visits(const char *path) {
  char *buf = file_read_all(path, NULL);
  if (!buf)
    return 0;
  unsigned long long v = strtoull(buf, NULL, 10);
  free(buf);
  return v;
}

static void persist_visits(const char *path, unsigned long long v) {
  char tmp[PATH_MAX];
  snprintf(tmp, sizeof tmp, "%s.tmp", path);
  FILE *f = fopen(tmp, "wb");
  if (!f)
    return;
  fprintf(f, "%llu\n", v);
  fclose(f);
  rename(tmp, path);
}

void visits_init(const char *counter_path) {
  g_counter_path = strdup(counter_path);
  g_visits = load_visits(g_counter_path);
}

unsigned long long visits_bump(void) {
  unsigned long long v;
  pthread_mutex_lock(&g_visits_lock);
  g_visits++;
  v = g_visits;
  persist_visits(g_counter_path, v);
  pthread_mutex_unlock(&g_visits_lock);
  return v;
}

unsigned long long visits_current(void) {
  unsigned long long v;
  pthread_mutex_lock(&g_visits_lock);
  v = g_visits;
  pthread_mutex_unlock(&g_visits_lock);
  return v;
}

void visits_format(unsigned long long v, char *out, size_t out_size) {
  char digits[32];
  int n = snprintf(digits, sizeof digits, "%llu", v);
  int commas = (n - 1) / 3;
  int total = n + commas;
  if ((size_t)total >= out_size)
    total = (int)out_size - 1;
  out[total] = '\0';
  int oi = total - 1;
  int di = n - 1;
  int count = 0;
  while (di >= 0 && oi >= 0) {
    out[oi--] = digits[di--];
    count++;
    if (count % 3 == 0 && di >= 0)
      out[oi--] = ',';
  }
}
