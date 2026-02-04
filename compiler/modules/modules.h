#pragma once
#ifndef ALBEDO_MODULES_H
#define ALBEDO_MODULES_H

#include "types.h"

#define INIT_MODULE_CAP 32

void init_module_loader(void);

void resolve_modules(void);

#endif // !ALBEDO_MODULES_H
