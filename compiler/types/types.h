#pragma once
#ifndef ALBEDO_TYPES_TYPES_H
#define ALBEDO_TYPES_TYPES_H

#include "../utils/types.h"

#define X_TYPES(X)  \
    X(Ty_Struct)    \
    X(Ty_Enum)      \
                    \
    X(Ty_Pointer)   \
    X(Ty_Array)     \
    X(Ty_Slice)     \
                    \
    X(Ty_Void)      \
    X(Ty_Bool)      \
                    \
    X(Ty_I8)        \
    X(Ty_I16)       \
    X(Ty_I32)       \
    X(Ty_I64)       \
                    \
    X(Ty_U8)        \
    X(Ty_U16)       \
    X(Ty_U32)       \
    X(Ty_U64)       \

typedef enum {
    X_TYPES(GENERATE_ENUM)
} TypeKind;

static const char* TYPE_KIND_STRINGS[] = {
    X_TYPES(GENERATE_STRING)
};

typedef struct Type {
    TypeKind kind;

    usize size;
    usize align;

    union {
        struct {
            struct Type* target;
        } pointer;
    } as;
} Type;

#endif // !ALBEDO_TYPES_TYPES_H
