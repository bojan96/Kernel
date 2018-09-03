#pragma once 
#include <stddef.h>
size_t util_strlen(const char* str);
void util_memcpy(void* dest, const void* src, size_t num);
void util_memmove(void* dest, const void* src, size_t num);
const char* util_itoa(int num);
