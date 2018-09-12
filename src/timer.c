#include "timer.h"
#include "io.h"

#define CLOCK_RATE 1193182
// Minimum allowed frequency is 19 HZ, Max 1193182 HZ
int timer_init(int frequency)
{
	if(frequency < 19 || frequency > 1193182)
		return 0;
	
	// Setup timer mode 
	writePortByte(0x43, 0x34);
	int count = CLOCK_RATE / frequency;
	writePortByte(0x40, count & 0xFF);
	writePortByte(0x40, count >> 8);
	
	return 1;
}

