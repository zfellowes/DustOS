[BITS 32]

global _start

extern kernel_main

section .text

_start:
	cli

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov edi, 0xB8000
	mov ecx, 80 * 25
	mov eax, 0x0720

.clear_loop:
	mov [edi], ax
	add edi, 2
	loop .clear_loop

	call kernel_main

.halt:
	hlt
	jmp .halt
