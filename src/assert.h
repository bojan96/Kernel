#pragma once 
#include <stdbool.h>

#ifdef NDEBUG
#define assert(expr)
#else
#define assert(expr) assert_assert(expr, #expr, __FILE__, __LINE__)
#endif

void assert_assert(bool condition,const char* expression,const char* file, int line);
