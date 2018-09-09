#include "terminal.h"
#include "assert.h"
#include "multiboot.h"
#include "allocator.h"
#include <stdint.h>
#include <stdbool.h>


void kernel_main(uint32_t multibootMagic, multiboot_info_t *multibootInfoAddr) 
{
	terminal_initialize();
	terminal_write("Hello, kernel World!\n");
	assert(multibootMagic == MULTIBOOT_BOOTLOADER_MAGIC);
	allocator_init(multibootInfoAddr);
	
	char str[] = "myStr";
	void *ptr1 = allocator_allocateBlocks(3);
	void *ptr2 = allocator_allocateBlocks(2);
	void *ptr3 = allocator_allocateBlocks(3);
	allocator_freeBlocks(ptr2, 2);
	void *ptr4 = allocator_allocateBlocks(2);
	void *ptr5 = allocator_allocateBlocks(64);
	assert(ptr5 == 0);
	allocator_freeBlocks(ptr1, 3);
	allocator_freeBlocks(ptr3, 3);
	allocator_freeBlocks(ptr4, 2);

	void *ptr6 = allocator_allocateBlocks(64);
	assert(ptr6 != 0);
	
	terminal_writeFormat("Num: %d %s %d\n", -105, str, 225); 
	terminal_writeFormat("Num: %d %s %d'n", -105, str, 225); 
	assert(3 > 2);
	
	for(int i = 0; i < 10; ++i)
	{
		terminal_writeFormat("%d:dasfadsfasdfsadf\n", i);
	}

	while(1);
}
