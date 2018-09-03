#include "terminal.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

typedef enum {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} VgaColor;


static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color;
static uint16_t* terminal_buffer = (uint16_t*)0xB8000;


static uint8_t vgaColor(VgaColor fg, VgaColor bg) 
{
	return fg | bg << 4;
}
 
static uint16_t vgaEntry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize(void) 
{
	/*terminal_row = 0;
	terminal_column = 0;*/
	terminal_color = vgaColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	//terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			 size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vgaEntry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
static void terminal_putEntry(char c, uint8_t color, size_t x, size_t y) 
{
	size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vgaEntry(c, color);
}
 
void terminal_putchar(char ch) 
{
	if(ch == '\n')
	{
		++terminal_row;
		terminal_column = 0;
		return;
	}
	
	terminal_putEntry(ch, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) 
	{
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}
 
static void terminal_writeData(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_write(const char* text) 
{
	terminal_writeData(text, util_strlen(text));
}

void terminal_writeLine(const char* text)
{
	terminal_write(text);
	terminal_putchar('\n');
}

void terminal_writeNum(int num)
{
	terminal_write(util_itoa(num));
}

char formattedStr[255];
void terminal_writeFormat(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	util_writeArgsStr(formattedStr, format, args);
	terminal_write(formattedStr);
	va_end(args);
}
