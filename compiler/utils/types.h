#pragma once
#ifndef TYPES_H
#define TYPES_H

/*
*
*   File structure: 
*       1. Primitive types
*       2. Type limits
*       3. X-Macro helper for enums
* 
*/


#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*
*
*   1. Primitive type aliases
*
*/

#define nullptr     ((void*)0)
#define null        ((void*)0)

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef uint8_t     b8;
typedef uint32_t    b32;

typedef float       f32;
typedef double      f64;
typedef long double f128;

typedef size_t      usize;
typedef ssize_t     isize;

/*
*
*   2. Type limits
*
*/

#define U8_MAX  UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define I8_MAX  INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX

#define USIZE_MAX SIZE_MAX 
#define ISIZE_MAX SSIZE_MAX 

/*
*
*   3. X-Macro helper for enums
*
*/

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#endif // !TYPES_H
