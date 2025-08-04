[BITS 32]

global _start

extern kernel_main
extern init_paging

section .text

_start:
	cli

	; Setup a simple stack
	mov esp, 0x9FF00
	and esp, 0xFFFFFFF0

	call init_paging
	call kernel_main

.halt:
	hlt
	jmp .halt
