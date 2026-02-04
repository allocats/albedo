#pragma once
#ifndef ALBEDO_HASH_H
#define ALBEDO_HASH_H

#include "../utils/types.h"

u32 fnv1a_hash(const char* ptr, const usize len);

#endif // !ALBEDO_HASH_H
