#ifndef LINXC_H
#define LINXC_H

#ifndef NULL
#define NULL 0
#endif

#define trait struct
#define def_delegate(name, returns, ...) typedef returns (*name)(__VA_ARGS__)
#define impl(name)
#define IsAttribute

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
typedef unsigned long long usize;

typedef const char *text;

#endif