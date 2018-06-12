BUILD_DIR = build
SRC_DIR = src
OBJ = $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o 

all:$(BUILD_DIR)/kernel


$(BUILD_DIR)/kernel:$(OBJ) linker.ld
	i686-elf-gcc -T linker.ld -o $(BUILD_DIR)/os.bin -ffreestanding -O2 -nostdlib $(OBJ) -lgcc

$(BUILD_DIR)/kernel.o:$(SRC_DIR)/kernel.c
	i686-elf-gcc -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

$(BUILD_DIR)/boot.o:$(SRC_DIR)/boot.asm
	nasm -felf32 $(SRC_DIR)/boot.asm -o $(BUILD_DIR)/boot.o


clean:
	rm build/*
