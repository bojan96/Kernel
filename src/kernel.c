#include "terminal.h"

 
void kernel_main(void) 
{
	terminal_initialize();
	terminal_write("Hello, kernel World!\n");
	terminal_write("Bla");
	for(int i = 0; i < 10; ++i)
	{
		terminal_writeLine("Bla");
	}
	
	while(1);
}
