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


// Returns 1 if registration was succesful, 0 otherwise
int RegisterHandler(int irqNum, IRQHandler handler)
{
	if(irqNum >= 16)
		return 0;
	
	handlers[irqNum] = handler;
	return 1;
}



