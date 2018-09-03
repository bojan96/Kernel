#include "terminal.h"
#include "assert.h"
 
void kernel_main(void) 
{
	terminal_initialize();
	terminal_write("Hello, kernel World!\n");
	terminal_write("Bla");
	for(int i = 0; i < 10; ++i)
	{
		terminal_writeFormat("%d, Bla\n", i);
	}
	
	char str[] = "myStr";
	terminal_writeFormat("Num: %d %s %d\n", -105, str, 225); 
	terminal_writeFormat("Num: %d %s %d", -105, str, 225); 
	assert(3 > 2);
	assert(2 > 3);
	while(1);
}
