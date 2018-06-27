
PIC_EOI equ 0x20

%macro OUT 2
	mov ax, %2
	out %1, ax
%endmacro

%macro DECLARE_IRQ 1
irq%1:
	mov [irq_code], %1
	jmp irq_common
%endmacro

section .data

irq_code dd 0

section .text

remap_irq:
	OUT 0x20, 0x11
	OUT 0xa0, 0x11
	OUT 0x21, 0x20
	OUT 0xa1, 0x28
	OUT 0x21, 0x04
	OUT 0xa1, 0x02
	OUT 0x21, 0x01
	OUT 0xa1, 0x01
	OUT 0x21, 0
	OUT 0xa1, 0
	ret

extern irq_common_high_level
irq_common:
	pushad ; save all GPR
	push dword [irq_code]
	call irq_common_high_level
	cmp dword [irq_code], 8  ; if irq code less than 8 do not send EOI to pic slave
	jl not_pic2
	OUT 0xa0, PIC_EOI

not_pic2:
	OUT 0x20, PIC_EOI  ; send EOI to master

	popad ; restore GPR
	iret

