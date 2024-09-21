#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>

#ifndef Allocate
#define Allocate(type, count) (type *)malloc(sizeof(type) * count)
#endif

#endif