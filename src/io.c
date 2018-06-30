#include "io.h"


void writeToPort(int16_t port, int32_t data)
{
	__asm__ volatile
	(
	"out %0, %1\n\t"::
	"d"(port), "a"(data):
	);
}

int32_t readFromPort(int16_t port)
{
	int32_t value;
	__asm__ volatile
	(
	"in %0, %1\n\t":
	"=a"(value):
	"d"(port):
	);
	
	return value;		
}
