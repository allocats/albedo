#pragma once
#ifndef ALBEDO_TYPE_SYSTEM_H
#define ALBEDO_TYPE_SYSTEM_H

#include "types.h"

#define INIT_TABLE_ENTRY_CAPACITY 64 

#define register_builtin_type(s, size, align) register_type(Ty_Builtin, s, sizeof(s) - 1, size, align)

void init_type_table(void);
void type_table_insert(TypeEntry* entry);
TypeEntry* type_table_lookup(u32 hash, char* ptr, usize len);
TypeEntry* register_type(TypeKind kind, char* ptr, usize len, u32 size, u16 alignment);
void register_builtin_types(void);

#endif // !ALBEDO_TYPE_SYSTEM_H
