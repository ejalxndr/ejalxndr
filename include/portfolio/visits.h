#ifndef PORTFOLIO_VISITS_H
#define PORTFOLIO_VISITS_H

#include <stddef.h>

void visits_init(const char *counter_path);
unsigned long long visits_bump(void);
unsigned long long visits_current(void);
void visits_format(unsigned long long v, char *out, size_t out_size);

#endif
