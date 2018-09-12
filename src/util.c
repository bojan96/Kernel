#include "util.h"
#include "assert.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>


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


// TODO: Avoid duplicated code
static char result[12];

static const char* util_llitoa(long long num);

const char* util_itoa(int num)
{
	return util_llitoa(num);
}

static const char* util_llitoa(long long num)
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
	long long toCount = num;
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


static const char* util_lluitoa(unsigned long long num)
{
	if(num == 0)
	{
		result[0] = '0';
		result[1] = '\0';
		return result;
	}
	
	unsigned long long toCount = num;
	int charCount = 0;
	
	while(toCount != 0)
	{
		++charCount;
		toCount/= 10;
	}
	
	int i = 1;
	result[charCount] = '\0';
	
	while(num != 0)
	{
		result[charCount - i++] = 0x30 + num % 10;
		num /= 10;
	}
	
	return result;
}

void util_writeArgsStr(char* str, const char* format, va_list args)
{
	while(*format)
	{
		switch(*format)
		{
			case '%':
				{
					const char* toCopy;
					int strLen;
					char percentSign = '%';
					
					switch(*++format)
					{
						case 'd':
							toCopy = util_llitoa(va_arg(args, int));
							strLen = util_strlen(toCopy);
							break;
						case 's':
							toCopy = va_arg(args, const char*);
							strLen = util_strlen(toCopy);
							break;
						case 'l':
							switch(*++format)
							{
								case 'l':
									switch(*++format)
									{
										case 'd':
											toCopy = util_llitoa(va_arg(args, long long int));
											strLen = util_strlen(toCopy);
										break;
										case 'u':
											toCopy = util_lluitoa(va_arg(args, unsigned long long int));
											strLen = util_strlen(toCopy);
											break;
										default:
											// Unreachable
											assert(false);
											break;
									}
									break;
								break;
								default:
									// Unreachable
									assert(false);
									break;
							}
							break;
							
						case 'u':
							toCopy = util_lluitoa(va_arg(args, unsigned int));
							strLen = util_strlen(toCopy);
							break;
						case '%':
							toCopy = &percentSign;
							strLen = 1;
							break;
						default:
							assert(false);
							break;
					}
					util_memcpy(str, toCopy, strLen);
					str += strLen;
				}
				break;
			default:
				*str++ = *format;
				break;
		}
		
		++format;
	}
	*str = '\0';
}

void util_memset(void *ptr, int ch, size_t num)
{
	uint8_t *bytePtr = (uint8_t *)ptr;
	
	while(num--)
	{
		*bytePtr++ = (uint8_t)ch; 
	}
}


