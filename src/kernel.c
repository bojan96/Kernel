#include "terminal.h"
#include "assert.h"
#include "multiboot.h"
#include "allocator.h"
#include "thread.h"
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>


Lock lock;

void threadProc2();

void threadProc()
{
	for(int num = 0; num < 10000; ++num)
	{
		thread_lock(&lock);
		terminal_writeFormat("Thread(%d): %d\n", thread_this(), num);
		thread_unlock(&lock);
	}
	
	Thread* thread = thread_createThread(threadProc2);
	assert(thread != 0);
	thread_join(thread);
}

void threadProc2()
{
	int prime;
	
	for(int num = 3; num < 10000; ++num)
	{
		prime = 1;
		
		for(int i = 2; i < num; ++i)
		{
			if(num % i == 0)
			{
				prime = 0;
				break;
			}
		}
		
		if(prime)
		{
			terminal_writeFormat("Thread(%d), Prime found: %d\n",thread_this(), num);
		}
	}
}



void kernel_main(uint32_t multibootMagic, multiboot_info_t *multibootInfoAddr) 
{
	terminal_initialize();
	terminal_write("Hello, kernel World!\n");
	assert(multibootMagic == MULTIBOOT_BOOTLOADER_MAGIC);
	allocator_init(multibootInfoAddr);
	thread_init();
	
	thread_initLock(&lock);
	Thread* thread = thread_createThread(threadProc);
	assert(thread != 0);
	
	for(int num = 0; num < 10; ++num)
	{
		thread_lock(&lock);
		terminal_writeFormat("Thread(%d): %d\n",thread_this(), num);
		thread_sleep(1000);
		thread_unlock(&lock);
	}
	
	thread_join(thread);
	
	
	


	while(1);
}
