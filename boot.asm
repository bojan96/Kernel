; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

CODE_SELECTOR equ 0x08
DATA_SELECTOR equ 0x10
IDT_FLAGS equ 0x8e

; 48 = 32 resrved + 16 IRQs
IDT_SIZE equ (48*8 - 1) ;in bytes, points to last valid byte
GDT_SIZE equ (3*8 - 1) ; same as above

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
 
section .data

gdt_reg db 0,0,0,0,0,0 ; 6 bytes
align 8

; Descriptors:
null_descriptor dd 0,0
code_descriptor db 0xff,0xff,0,0,0,0x9a,0xcf,0
data_descriptor db 0xff,0xff,0,0,0,0x92,0xcf,0

idt_reg db 0,0,0,0,0,0 ; 6 bytes

align 8

; 256 entries, each 8 bytes, initalize to zero,
idt times 2048 db 0

section .bss

align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:


section .text

global _start:function (_start.end - _start)
_start:
	cli ; Disable interrupts	
	mov esp, stack_top
	call gdt_init ; Setup GDT
	call idt_init ; Setup IDT 
	;call setup_irq ; remap irqs, setup idt entries for irqs
	;sti ; Enable interrupts
	;int 1

; Go into infinite loop
;.loop:	nop
;	jmp .loop

	extern kernel_main
	call kernel_main

	cli
.hang:	hlt
	jmp .hang
.end:


gdt_init:
	; null_descriptor is base address for gdt
	mov eax, null_descriptor
	mov bx, GDT_SIZE

	mov [gdt_reg], bx ; Setup GDT limit
	mov [gdt_reg + 2], eax ; Setup GDT base
	lgdt [gdt_reg]

	;Setup segment registers
	mov cx, DATA_SELECTOR
	mov ss, cx
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx

	jmp CODE_SELECTOR:exit_init
exit_init:
	ret

%macro IDT_ENTRY_SETUP 1
	mov eax, interrupt
	mov [idt + %1*8], ax; offset, lower 16 bits
	shr ax, 16
	mov word [idt + %1*8 + 2], CODE_SELECTOR ; segment selector
	mov byte [idt + %1*8 + 4], 0 ; always zero
	mov byte [idt + %1*8 + 5], IDT_FLAGS ; flags
	mov [idt + %1*8 + 6], ax ;offset, higher 16 bits

%endmacro

%macro IDT_ENTRY_SETUP_ERR 1
	mov eax, interrupt_error
	mov [idt + %1*8], ax; offset, lower 16 bits
	shr ax, 16
	mov word [idt + %1*8 + 2], CODE_SELECTOR ; segment selector
	mov byte [idt + %1*8 + 4], 0 ; always zero
	mov byte [idt + %1*8 + 5], IDT_FLAGS ; flags
	mov [idt + %1*8 + 6], ax ;offset, higher 16 bits

%endmacro

%macro IDT_ENTRY_IRQ_MASTER 1
	mov eax, irq_master
	mov [idt + %1*8], ax; offset, lower 16 bits
	shr ax, 16
	mov word [idt + %1*8 + 2], CODE_SELECTOR ; segment selector
	mov byte [idt + %1*8 + 4], 0 ; always zero
	mov byte [idt + %1*8 + 5], IDT_FLAGS ; flags
	mov [idt + %1*8 + 6], ax ;offset, higher 16 bits
%endmacro

%macro IDT_ENTRY_IRQ_SLAVE 1
	mov eax, irq_slave
	mov [idt + %1*8], ax; offset, lower 16 bits
	shr ax, 16
	mov word [idt + %1*8 + 2], CODE_SELECTOR ; segment selector
	mov byte [idt + %1*8 + 4], 0 ; always zero
	mov byte [idt + %1*8 + 5], IDT_FLAGS ; flags
	mov [idt + %1*8 + 6], ax ;offset, higher 16 bits
%endmacro


idt_init:
	IDT_ENTRY_SETUP 0
	IDT_ENTRY_SETUP 1
	IDT_ENTRY_SETUP 2
	IDT_ENTRY_SETUP 3
	IDT_ENTRY_SETUP 4
	IDT_ENTRY_SETUP 5
	IDT_ENTRY_SETUP 6
	IDT_ENTRY_SETUP 7
	IDT_ENTRY_SETUP_ERR 8
	IDT_ENTRY_SETUP 9
	IDT_ENTRY_SETUP_ERR 10
	IDT_ENTRY_SETUP_ERR 11
	IDT_ENTRY_SETUP_ERR 12
	IDT_ENTRY_SETUP_ERR 13
	IDT_ENTRY_SETUP_ERR 14
	IDT_ENTRY_SETUP 15
	IDT_ENTRY_SETUP 16
	IDT_ENTRY_SETUP_ERR 17
	IDT_ENTRY_SETUP 18
	IDT_ENTRY_SETUP 19
	IDT_ENTRY_SETUP 20
	IDT_ENTRY_SETUP 21
	IDT_ENTRY_SETUP 22
	IDT_ENTRY_SETUP 23
	IDT_ENTRY_SETUP 24
	IDT_ENTRY_SETUP 25
	IDT_ENTRY_SETUP 26
	IDT_ENTRY_SETUP 27
	IDT_ENTRY_SETUP 28
	IDT_ENTRY_SETUP 29
	IDT_ENTRY_SETUP 30
	IDT_ENTRY_SETUP 31
	mov word [idt_reg], IDT_SIZE
	mov dword [idt_reg + 2], idt
	lidt [idt_reg]
	ret

%macro portW 2
	mov al, %2
	out %1, al
%endmacro

remap_irq:
	; Remap IRQs
	portW 0x20, 0x11
        portW 0xa0, 0x11
        portW 0x21, 0x20
        portW 0xa1, 0x28
        portW 0x21, 0x04
        portW 0xa1, 0x02
        portW 0x21, 0x01
        portW 0xa1, 0x01
        portW 0x21, 0x0
        portW 0xa1, 0x0
	ret

setup_irq:
	call remap_irq
	IDT_ENTRY_IRQ_MASTER 32
	IDT_ENTRY_IRQ_MASTER 33
	IDT_ENTRY_IRQ_MASTER 34
	IDT_ENTRY_IRQ_MASTER 35
	IDT_ENTRY_IRQ_MASTER 36
	IDT_ENTRY_IRQ_MASTER 37
	IDT_ENTRY_IRQ_MASTER 38
	IDT_ENTRY_IRQ_MASTER 39
	IDT_ENTRY_IRQ_SLAVE 40
	IDT_ENTRY_IRQ_SLAVE 41
	IDT_ENTRY_IRQ_SLAVE 42
	IDT_ENTRY_IRQ_SLAVE 43
	IDT_ENTRY_IRQ_SLAVE 44
	IDT_ENTRY_IRQ_SLAVE 45
	IDT_ENTRY_IRQ_SLAVE 46
	IDT_ENTRY_IRQ_SLAVE 47
	ret

interrupt:
	iret

interrupt_error:
	add esp, 4
	iret

irq_master:
	portW 0x20, 0x20
	iret

irq_slave:
	portW 0xa0, 0x20
	portW 0x20, 0x20
	iret

