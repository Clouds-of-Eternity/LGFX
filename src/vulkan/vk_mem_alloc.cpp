#include "volk.h"

#ifdef DEBUG
#include <stdio.h>

#define VMA_LEAK_LOG_FORMAT(format, ...) printf(format, __VA_ARGS__)
#endif

#define VMA_IMPLEMENTATION
#include "vulkan/vk_mem_alloc.h"