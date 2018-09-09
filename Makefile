BUILD_DIR = build
SRC_DIR = src
OBJ = $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/irq.o $(BUILD_DIR)/irq_high_level.o\
$(BUILD_DIR)/isr_high_level.o $(BUILD_DIR)/isr.o $(BUILD_DIR)/io.o $(BUILD_DIR)/timer.o \
$(BUILD_DIR)/terminal.o $(BUILD_DIR)/util.o $(BUILD_DIR)/assert.o $(BUILD_DIR)/allocator.o \
$(BUILD_DIR)/thread.o
CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra -masm=intel -D DEBUG_ALLOCATOR

all:$(BUILD_DIR)/kernel


$(BUILD_DIR)/kernel:$(OBJ) linker.ld
	i686-elf-gcc -T linker.ld -o $(BUILD_DIR)/os.bin -ffreestanding -O2 -nostdlib $(OBJ) -lgcc

$(BUILD_DIR)/kernel.o : $(SRC_DIR)/kernel.c $(SRC_DIR)/multiboot.h $(SRC_DIR)/assert.h $(SRC_DIR)/terminal.h $(SRC_DIR)/allocator.h
	i686-elf-gcc -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o $(CFLAGS)

$(BUILD_DIR)/boot.o:$(SRC_DIR)/boot.asm
	nasm -felf32 $(SRC_DIR)/boot.asm -o $(BUILD_DIR)/boot.o

$(BUILD_DIR)/irq.o:$(SRC_DIR)/irq.asm
	nasm -felf32 $(SRC_DIR)/irq.asm -o $(BUILD_DIR)/irq.o

$(BUILD_DIR)/isr.o:$(SRC_DIR)/isr.asm
	nasm -felf32 $(SRC_DIR)/isr.asm -o $(BUILD_DIR)/isr.o

$(BUILD_DIR)/irq_high_level.o : $(SRC_DIR)/irq_high_level.c $(SRC_DIR)/irq_high_level.h
	i686-elf-gcc -c $(SRC_DIR)/irq_high_level.c -o $(BUILD_DIR)/irq_high_level.o $(CFLAGS)

$(BUILD_DIR)/isr_high_level.o:$(SRC_DIR)/isr_high_level.c
	i686-elf-gcc -c $(SRC_DIR)/isr_high_level.c -o $(BUILD_DIR)/isr_high_level.o $(CFLAGS)
	
$(BUILD_DIR)/io.o : $(SRC_DIR)/io.c $(SRC_DIR)/io.h
	i686-elf-gcc -c $(SRC_DIR)/io.c -o $(BUILD_DIR)/io.o $(CFLAGS)

$(BUILD_DIR)/timer.o : $(SRC_DIR)/timer.c $(SRC_DIR)/timer.h $(SRC_DIR)/io.h
	i686-elf-gcc -c $(SRC_DIR)/timer.c -o $(BUILD_DIR)/timer.o $(CFLAGS)
	
$(BUILD_DIR)/util.o : $(SRC_DIR)/util.c $(SRC_DIR)/util.h $(SRC_DIR)/assert.h
	i686-elf-gcc -c $(SRC_DIR)/util.c -o $(BUILD_DIR)/util.o $(CFLAGS)
	
$(BUILD_DIR)/terminal.o : $(SRC_DIR)/terminal.c $(SRC_DIR)/terminal.h $(SRC_DIR)/util.h
	i686-elf-gcc -c $(SRC_DIR)/terminal.c -o $(BUILD_DIR)/terminal.o $(CFLAGS)
	
$(BUILD_DIR)/assert.o : $(SRC_DIR)/assert.c $(SRC_DIR)/assert.h
	i686-elf-gcc -c $(SRC_DIR)/assert.c -o $(BUILD_DIR)/assert.o $(CFLAGS)
	
$(BUILD_DIR)/allocator.o : $(SRC_DIR)/allocator.c $(SRC_DIR)/allocator.h $(SRC_DIR)/multiboot.h $(SRC_DIR)/assert.h $(SRC_DIR)/terminal.h $(SRC_DIR)/util.h
	i686-elf-gcc -c $(SRC_DIR)/allocator.c -o $(BUILD_DIR)/allocator.o $(CFLAGS)
	
$(BUILD_DIR)/thread.o : $(SRC_DIR)/thread.c $(SRC_DIR)/thread.h
	i686-elf-gcc -c $(SRC_DIR)/thread.c -o $(BUILD_DIR)/thread.o $(CFLAGS)
	
clean:
	rm build/*
