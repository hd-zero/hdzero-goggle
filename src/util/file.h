#pragma once

#include <stdbool.h>

bool file_compare(char *f1, char *f2);
bool file_exists(const char *filename);
bool file_printf(const char *filename, const char *fmt, ...);
long file_get_size(const char* filename);