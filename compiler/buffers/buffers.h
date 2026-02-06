#pragma once
#ifndef ALBEDO_BUFFERS_H
#define ALBEDO_BUFFERS_H

#include "../albedo/albedo.h"

#define FB_GROWTH_FACTOR 1.25

bool map_file(const char* path);
void buffer_cleanup();

void file_buffers_append(char* paths[], usize count);

#endif // !ALBEDO_BUFFERS_H
