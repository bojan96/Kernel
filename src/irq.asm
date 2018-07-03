
PIC_EOI equ 0x20

%macro OUT 2
	mov ax, %2
	out %1, ax
%endmacro

%macro DECLARE_IRQ 1
global irq_handler_%1
irq_handler_%1:
	mov dword [irq_code], %1
	jmp irq_common
%endmacro

section .data

irq_code dd 0

section .text
global remap_irq
remap_irq:
	OUT 0x20, 0x11
	OUT 0xa0, 0x11
	OUT 0x21, 0x20
	OUT 0xa1, 0x28
	OUT 0x21, 0x04
	OUT 0xa1, 0x02
	OUT 0x21, 0x01
	OUT 0xa1, 0x01
	OUT 0x21, 0xfe
	OUT 0xa1, 0xff
	ret

extern irq_common_high_level
irq_common:
	pushad ; save all GPR
	push dword [irq_code]
	call irq_common_high_level
	pop ecx
	cmp dword [irq_code], 8  ; if irq code less than 8 do not send EOI to pic slave
	jl not_pic2
	OUT 0xa0, PIC_EOI

not_pic2:
	OUT 0x20, PIC_EOI  ; send EOI to master

	popad ; restore GPR
	iret

DECLARE_IRQ 0
DECLARE_IRQ 1
DECLARE_IRQ 2
DECLARE_IRQ 3
DECLARE_IRQ 4
DECLARE_IRQ 5
DECLARE_IRQ 6
DECLARE_IRQ 7
DECLARE_IRQ 8
DECLARE_IRQ 9
DECLARE_IRQ 10
DECLARE_IRQ 11
DECLARE_IRQ 12
DECLARE_IRQ 13
DECLARE_IRQ 14
DECLARE_IRQ 15
