#include "timer.h"
#include "io.h"
#include <stdint.h>

#define CLOCK_RATE 1193182
// Minimum allowed frequency is 19 HZ, Max 1193182 HZ
void initTimer(uint16_t frequency)
{
	// Setup timer mode 
	writePortByte(0x43, 0x34);
	int count = CLOCK_RATE / frequency;
	writePortByte(0x40, count & 0xFF);
	writePortByte(0x40, count >> 8);
}

