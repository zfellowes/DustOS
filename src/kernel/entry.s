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
	
	; Clear the screen
	mov edi, 0xB8000
	mov ecx, 80 * 25
	mov eax, 0x0720

.clear_loop:
	stosw
	loop .clear_loop

	call kernel_main ; kernel.c

.halt:
	hlt
	jmp .halt
