#pragma once
#include <stdint.h>

void terminal_write(const char* text);
void terminal_writeLine(const char* text);
void terminal_initialize(void);
void terminal_putchar(char ch);
void terminal_setcolor(uint8_t color);
