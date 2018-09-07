#pragma once
#include "multiboot.h"
#include <stdint.h>

#define BLOCK_SIZE 4096
void allocator_init(multiboot_info_t *multibootInfo);
void *allocator_allocateBlocks(uint32_t num);
