#pragma once
#include <stdint.h>

void terminal_writestring(const char* text);
void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_setcolor(uint8_t color);
