#include "hash.h"

u32 fnv1a_hash(const char* ptr, const usize len) {
    u32 hash = 2166136261u;

    for (u32 i = 0; i < len; i++) {
        hash ^= ptr[i];
        hash *= 16777619u;
    }

    return hash;
} 
