#pragma once
#ifndef TYPES_H
#define TYPES_H

/*
*
*   File structure: 
*       1. Primitive types
*       2. X-Macro helper for enums
* 
*/


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*
*
*   1. Primitive type aliases
*
*/

#define nullptr     ((void*)0)

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
typedef ssize_t     ssize;

/*
*
*   2. X-Macro helper for enums
*
*/

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#endif // !TYPES_H

