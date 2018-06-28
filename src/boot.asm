; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

CODE_SELECTOR equ 0x08
DATA_SELECTOR equ 0x10
IDT_FLAGS equ 0x8e

; 32 reserved idt entries + 16 irq handlers
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

extern remap_irq

section .text

global _start:function (_start.end - _start)
_start:
	cli ; Disable interrupts	
	mov esp, stack_top
	call gdt_init ; Setup GDT
	call remap_irq
	call idt_init ; Setup IDT
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
	extern isr_handler_%1
	mov eax, isr_handler_%1
	mov [idt + %1*8], ax; offset, lower 16 bits
	shr eax, 16
	mov word [idt + %1*8 + 2], CODE_SELECTOR ; segment selector
	mov byte [idt + %1*8 + 4], 0 ; always zero
	mov byte [idt + %1*8 + 5], IDT_FLAGS ; flags
	mov [idt + %1*8 + 6], ax ;offset, higher 16 bits

%endmacro

%macro IDT_ENTRY_SETUP_ERR 1
	mov eax, interrupt_error
	mov [idt + %1*8], ax; offset, lower 16 bits
	shr eax, 16
	mov word [idt + %1*8 + 2], CODE_SELECTOR ; segment selector
	mov byte [idt + %1*8 + 4], 0 ; always zero
	mov byte [idt + %1*8 + 5], IDT_FLAGS ; flags
	mov [idt + %1*8 + 6], ax ;offset, higher 16 bits

%endmacro

;first argument is idt entry index, second is irq number
%macro IDT_IRQ_SETUP 2
	extern irq_handler_%2
	mov eax, irq_handler_%2
	mov [idt + %1*8], ax; offset, lower 16 bits
	shr eax, 16
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
      	IDT_IRQ_SETUP 32, 0
	IDT_IRQ_SETUP 33, 1
	IDT_IRQ_SETUP 34, 2
	IDT_IRQ_SETUP 35, 3
	IDT_IRQ_SETUP 36, 4
	IDT_IRQ_SETUP 37, 5
	IDT_IRQ_SETUP 38, 6
	IDT_IRQ_SETUP 39, 7
	IDT_IRQ_SETUP 40, 8
	IDT_IRQ_SETUP 41, 9
	IDT_IRQ_SETUP 42, 10
	IDT_IRQ_SETUP 43, 11
	IDT_IRQ_SETUP 44, 12
	IDT_IRQ_SETUP 45, 13
	IDT_IRQ_SETUP 46, 14
	IDT_IRQ_SETUP 47, 15	

	; Fill IDTR with size and base
	mov word [idt_reg], IDT_SIZE
	mov dword [idt_reg + 2], idt
	lidt [idt_reg]
	ret

interrupt_error:
	add esp, 4
	iret

