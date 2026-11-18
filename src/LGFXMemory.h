#pragma once
#include <stdlib.h>

#define Allocate(type, count) (type *)malloc(sizeof(type) * (count))