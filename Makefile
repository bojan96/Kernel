all:kernel


kernel:kernel.o boot.o linker.ld
	i686-elf-gcc -T linker.ld -o os.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc


kernel.o:kernel.c
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

boot.o:boot.asm
	nasm -felf32 boot.asm -o boot.o

clean:
	rm *.o 
