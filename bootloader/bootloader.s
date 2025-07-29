[BITS 16]
[ORG 0x7C00]

jmp start

bootdrive db 0
msg_start db '[+] Booting...', 13, 10, 0
msg_reset db '[+] Resetting drive...', 13, 10, 0
msg_a20 db '[+] Enable a20...', 13, 10, 0
msg_kernel db '[+] Loading Kernel @ 0x07E00', 13, 10, 0
msg_gdt db '[+] Loading GDT @ 0x00500', 13, 10, 0
msg_pmode db '[+] Entering Protected Mode...', 13, 10, 0

GDTR:
GDTsize DW GDT_END - GDT - 1
GDTbase DD 0x500

GDT:
NULL_SEL EQU $ - GDT
	DD 0x0
	DD 0x0
CODESEL EQU $ - GDT
	DW 0xFFFF
	DW 0x0
	DB 0x0
	DB 0x9A
	DB 0xCF
	DB 0x0
DATASEL EQU $ - GDT
	DW 0xFFFF
	DW 0x0
	DB 0x0
	DB 0x92
	DB 0xCF
	DB 0x0
GDT_END:

print_char:
	mov ah, 0x0E
	mov bh, 0x00
	mov bl, 0x07
	int 0x10
	ret

print_string:
	nextc:
		mov al, [si]
		inc si
		cmp al, 0
		je exit
		call print_char
		jmp nextc
		exit: ret
	

error:
	hlt
	jmp error

start:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov [bootdrive], dl
	mov si, msg_start
	call print_string

reset_drive:
	mov si, msg_reset
	call print_string
	mov ax, 0x00
	mov dl, [bootdrive]
	int 13h
	jc error

loadkernel:
	mov si, msg_kernel
	call print_string
	mov ax, 0x07E0
	mov es, ax
	mov bx, 0x0000
	mov ah, 02
	mov al, 01
	mov ch, 00
	mov cl, 02
	mov dh, 00
	mov dl, [bootdrive]
	int 0x13
	jc error

a20:
	mov si, msg_a20
	call print_string
	cli
	mov ax, 0x2401
	int 0x15
	jc error
	mov ax, 0x2401
	int 0x15
	jc error

load_GDT:
	mov si, msg_gdt
	call print_string
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov si, GDT
	mov di, [GDTbase]
	mov cx, [GDTsize]
	cld
	rep movsb
	lgdt [GDTR]

enter_pmode:
	mov si, msg_pmode
	call print_string
	;mov ah, 3
	;int 0x10
	mov eax, cr0
	or al, 1
	mov cr0, eax
	jmp 08h:next

[BITS 32]
	next:
	mov eax, DATASEL
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	mov esp, 0xffff
	jmp CODESEL:0x7E00
	hlt

times 510 - ($ - $$) db 0
SIGNATURE dw 0AA55h
