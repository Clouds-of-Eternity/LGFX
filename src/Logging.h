#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#ifndef LGFX_ERROR
#define LGFX_ERROR(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef LGFX_WARN
#define LGFX_WARN(...) printf(__VA_ARGS__)
#endif

#endif