#pragma once
#ifndef ALBEDO_DIAGNOSTICS_H
#define ALBEDO_DIAGNOSTICS_H

#include "types.h"

void err_file_not_found(const char* path);
void err_cant_open_file(const char* path);
void err_cant_map_file(const char* path);

#endif // !ALBEDO_DIAGNOSTICS_H
