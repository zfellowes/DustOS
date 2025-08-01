[BITS 32]

global _start

extern kernel_main

section .text

_start:
	cli

	; No need to do all this, GRUB does it.
	;mov ax, 0x10
	;mov ds, ax
	;mov es, ax
	;mov fs, ax
	;mov gs, ax
	;mov ss, ax
	; Keep here in case removal of GRUB for custom bootloader!

	; Setup a simple stack
	mov esp, 0x9FF00
	and esp, 0xFFFFFFF0

	call kernel_main

.halt:
	hlt
	jmp .halt
