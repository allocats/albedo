#pragma once
#ifndef ALBEDO_MACROS_H
#define ALBEDO_MACROS_H

#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#endif // !ALBEDO_MACROS_H
