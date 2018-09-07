#include "allocator.h"
#include "multiboot.h"
#include "assert.h"
#include "terminal.h"
#include <stdint.h>

extern char SECTIONS_END;

static uintptr_t MemoryStart;
static uintptr_t MemorySize;
static uint64_t bitmap = 0; // at least 64 blocks required
#define MASK_LOW_BITS (BLOCK_SIZE - 1)
#define MASK_HIGH_BITS (~MASK_LOW_BITS)

void allocator_init(multiboot_info_t *multibootInfo)
{
	assert((multibootInfo->flags & (1 << 6)) != 0);

	uintptr_t memoryMapArray = (uintptr_t)multibootInfo->mmap_addr;
	uintptr_t memoryMapEnd = memoryMapArray + multibootInfo->mmap_length;
	size_t largestMemSize = 0;
	uintptr_t sectionEndAddr = (uintptr_t)&SECTIONS_END;
	
	terminal_writeFormat("SECTIONS_END: %d\n", sectionEndAddr);
	terminal_writeLine("Memory regions");
	while(memoryMapArray < memoryMapEnd)
	{
		multiboot_memory_map_t *memoryMapEntry = (multiboot_memory_map_t*)memoryMapArray;
		terminal_writeFormat("Addr: %lld, Len: %lld, Type: %d\n", 
			memoryMapEntry->addr, memoryMapEntry->len, memoryMapEntry->type);
		
		size_t regionSize = memoryMapEntry->addr + memoryMapEntry->len - sectionEndAddr;
		if(memoryMapEntry->type == MULTIBOOT_MEMORY_AVAILABLE  && memoryMapEntry->addr <= sectionEndAddr 
			&& sectionEndAddr < memoryMapEntry->addr + memoryMapEntry->len && regionSize > largestMemSize)
		{
			largestMemSize = regionSize;
		}
			
		memoryMapArray += memoryMapEntry->size + sizeof(memoryMapEntry->size);
	}
	
	// No suitable memory region found
	assert(largestMemSize != 0);
	
	terminal_writeFormat("Suitable memory region found addr: %d, Size: %d\n", sectionEndAddr, largestMemSize);
	
	// Round up to multiple of BLOCK_SIZE
	MemoryStart = sectionEndAddr + BLOCK_SIZE - (sectionEndAddr & MASK_LOW_BITS);
	assert(MemoryStart >= sectionEndAddr);
	assert(MemoryStart % BLOCK_SIZE == 0);  
	
	// Round down to multiple of BLOCK_SIZE
	MemorySize = ((sectionEndAddr + largestMemSize) & MASK_HIGH_BITS) - MemoryStart;
	assert(MemorySize % BLOCK_SIZE == 0);
	assert(MemorySize <= largestMemSize);
	assert(MemorySize / BLOCK_SIZE >= sizeof(bitmap) * 8);
	terminal_writeFormat("Memory region to be used with allocator addr: %d, Size %d\n", MemoryStart, MemorySize);
	
}


// Allocates in granularity of 4K
void *allocator_allocateBlocks(uint32_t num)
{
	if(num > sizeof(bitmap) * 8)
		return 0;

	uint64_t mask = (num == sizeof(bitmap) * 8 ? ~(uint64_t)(0) : (uint64_t)((1 << num) - 1));
	uint32_t shiftMax = sizeof(bitmap) * 8 - num + 1;
	uint32_t shift;
	
	for(shift = 0; shift < shiftMax; ++shift)
	{
		// Found free blocks
		if(((mask << shift) & bitmap) == 0)
		{
			// Mark blocks as used
			bitmap |= (mask << shift);
			break;
		}
		
	}
	
	// No block found
	if(shift == shiftMax)
		return 0;
			
	uintptr_t addr = MemoryStart + shift * BLOCK_SIZE;
	
	return (void*)(addr);
}


void freeBlocks(void *ptr, uint32_t blocks)
{
	uintptr_t addr = (uintptr_t)ptr;
	assert(addr >= MemoryStart);
	assert(addr < MemoryStart + MemorySize);
	assert(addr % BLOCK_SIZE == 0);
	assert(blocks <= sizeof(bitmap) * 8);
	
	uint64_t mask = (1 << blocks) - 1;
	int blockIndex = (addr - MemoryStart) / BLOCK_SIZE;
	
	// Mark blocks as free
	bitmap &= ~(mask << blockIndex);
	
}
