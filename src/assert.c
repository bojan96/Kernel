#include "assert.h"
#include "irq_high_level.h"
#include "terminal.h"

void assert_assert(bool condition,const char* expression,const char* file, int line)
{
	if(!condition)
	{
		terminal_clearScreen();
		terminal_writeFormat("Assertion \"%s\" failed, file: %s, line: %d", expression, file, line);
		disableInterrupts();
		__asm__ volatile ("hlt\n\t":::);	
	}
}
