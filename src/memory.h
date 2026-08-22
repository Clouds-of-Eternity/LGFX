#pragma once

#include <stdlib.h>

#ifndef Allocate
#define Allocate(type, count) (type *)malloc(sizeof(type) * (count))
#endif