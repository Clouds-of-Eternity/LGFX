#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <assert.h>

#ifndef LGFX_ERROR
#define LGFX_ERROR(...) fprintf(stderr, __VA_ARGS__); assert(false)
#endif

#ifndef LGFX_WARN
#define LGFX_WARN(...) printf(__VA_ARGS__)
#endif

#endif