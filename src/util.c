#include "util.h"
#include <stdint.h>
#include <stdbool.h>


size_t util_strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void util_memcpy(void* dest, const void* src, size_t num)
{
	const uint8_t* srcByte = (uint8_t*)src;
	uint8_t* destByte = (uint8_t*)dest;
	
	while(num-- > 0)
		*destByte++ = *srcByte++;
}


static void util_copyBackwards(void* dest, const void* src, size_t num)
{
	const uint8_t* srcByte = (uint8_t*)src;
	uint8_t* destByte = (uint8_t*)dest;
	
	while(num-- > 0)
	{
		destByte[num] = srcByte[num];
	}
}

void util_memmove(void* dest, const void* src, size_t num)
{
	uintptr_t srcAddress = (uintptr_t)src;
	uintptr_t destAddress = (uintptr_t)dest;
	
	if(srcAddress == destAddress)
		return;
	
	if(srcAddress < destAddress && srcAddress + num >= destAddress)
	{
		util_copyBackwards(dest, src, num);	
	}
	// memcpy copies forward
	else
		util_memcpy(dest, src, num);
}


static char result[12];
const char* util_itoa(int num)
{
	bool isNegative = false;
	
	if(num == 0)
	{
		result[0] = '0';
		result[1] = '\0';
		return result;
	}
	else if(num < 0)
	{
		num = -num;
		isNegative = true;
		result[0] = '-';
	}
	
	// Include '-' if negative
	int charCount = isNegative ? 1 : 0;
	int toCount = num;
	while(toCount != 0)
	{
		++charCount;
		toCount /= 10;
	}
	
	int i = 1;
	result[charCount] = '\0';
	while(num != 0)
	{
		// 0x30 is '0';
		result[charCount - i++] = 0x30 + num % 10;
		num /= 10;
	}
	
	return result;
}


