#include "io.h"


void writePortByte(uint16_t port, uint8_t data)
{
	__asm__ volatile
	(
	"out %0, %1\n\t"::
	"d"(port), "a"(data):
	);
}

uint8_t readPortByte(uint16_t port)
{
	int8_t value;
	__asm__ volatile
	(
	"in %0, %1\n\t":
	"=a"(value):
	"d"(port):
	);
	
	return value;		
}



