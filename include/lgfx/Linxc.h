#ifndef LINXC_H
#define LINXC_H

#include <stddef.h>

#ifndef NULL
#define NULL 0
#endif

#define def_delegate(name, returns, ...) typedef returns (*name)(__VA_ARGS__)
#define BeginExports()
#define EndExport()
#define suppressGC

#ifdef __cplusplus
#define exportC extern "C"
#else
#define exportC
#endif

#ifdef WINDOWS
#define exportDynamic __declspec(dllexport)
#define importDynamic __declspec(dllimport)
#else
#define exportDynamic __attribute__((visibility("default")))
#define importDynamic
#endif

#ifdef EXPORT_DYNAMIC_LIBRARY
#define DynamicFunction exportDynamic
#else
#define DynamicFunction importDynamic
#endif

typedef signed char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef size_t usize;

typedef const char *text;
typedef const char *u8text;

#endif