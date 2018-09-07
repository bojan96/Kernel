#pragma once
#include "multiboot.h"
#include <stddef.h>
#include <stdbool.h>

void allocator_init(multiboot_info_t *multibootInfo);
void *allocator_allocateBlocks(size_t num);
