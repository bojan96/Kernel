#include "irq_high_level.h"

extern void terminal_writestring(const char* data);

// All initialized to zero
IRQHandler handlers[16] = {0};

// Called from irq handlers defined in assembly
void irq_common_high_level(int irq_code)
{
	
	IRQHandler handler = handlers[irq_code];
	
	if(handler != 0)
		handler();
		
}





