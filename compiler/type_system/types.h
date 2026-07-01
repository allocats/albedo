#pragma once
#ifndef ALBEDO_TYPE_SYSTEM_TYPES_H
#define ALBEDO_TYPE_SYSTEM_TYPES_H

#include "../utils/types.h"

typedef struct TypeEntry TypeEntry;

#define TYPE_IS_RESOLVED (1 << 0)
#define TYPE_IS_SIGNED   (1 << 1)
#define TYPE_IS_PACKED   (1 << 2)

#define X_TYPES(X)  \
    X(Ty_Void)      \
    X(Ty_Bool)      \
                    \
    X(Ty_Int)       \
    X(Ty_Float)     \
                    \
    X(Ty_Pointer)   \
                    \
    X(Ty_Array)     \
    X(Ty_Slice)     \
                    \
    X(Ty_Struct)    \
    X(Ty_Union)     \
    X(Ty_Enum)      \

typedef enum __attribute__((packed)) {
    X_TYPES(GENERATE_ENUM)
} TypeKind;

static const char* TYPE_KIND_STRINGS[] = {
    X_TYPES(GENERATE_STRING)
};

#undef X_TYPES

typedef struct Type Type;
typedef struct Symbol Symbol;

typedef struct Field {
    char* ptr;
    usize len;

    Type* type;

    u32 offset;
} Field;

typedef struct Variant {
    char* ptr;
    usize len;
    i64 value;
} Variant;

typedef struct Type {
    TypeKind kind;

    u8 flags;

    usize size;
    usize alignment;

    char* name_ptr;
    usize name_len;

    union {
        struct {
            u8 bits;
        } as_primitive;

        struct {
            Type* base;
        } as_pointer;

        struct {
            Type* base;
        } as_slice;

        struct {
            Type* base_type;
            usize length;
        } as_array;

        struct {
            Symbol* symbol;

            Field* fields;
            usize field_count;
        } as_struct_or_union;

        struct {
            Symbol* symbol;

            Type* underlying_type;

            Variant* variants;
            usize variant_count;
        } as_enum;
    };
} Type;

#endif // !ALBEDO_TYPE_SYSTEM_TYPES_H
