# Makefile for x86 arch

NASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

CFLAGS = -m32 -ffreestanding -O0 -Wall -Wextra -fno-pic -fno-builtin -fno-stack-protector -Isrc/kernel/console
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386

BUILD = build
ISO_DIR = iso
KERNEL_ELF = $(BUILD)/kernel.elf
ISO = DustOS.iso

MULTIBOOT_HDR = src/bootloader/multiboot.s
ENTRY_ASM = src/kernel/arch/x86/entry.s
KERNEL_C = src/kernel/arch/x86/kernel.c
LINKER_SCRIPT = src/kernel/arch/x86/linker.ld

GRUB_CFG = src/bootloader/grub/grub.cfg

.PHONY: all clean run iso

all: $(ISO)

$(KERNEL_ELF): $(MULTIBOOT_HDR) $(ENTRY_ASM) $(KERNEL_C) $(LINKER_SCRIPT)
	mkdir -p $(BUILD)
	$(NASM) $(ASMFLAGS) $(MULTIBOOT_HDR) -o $(BUILD)/multiboot_header.o
	$(NASM) $(ASMFLAGS) $(ENTRY_ASM) -o $(BUILD)/entry.o
	$(CC) $(CFLAGS) -m32 -c $(KERNEL_C) -o $(BUILD)/kernel.o
	$(CC) $(CFLAGS) -m32 -c src/kernel/arch/x86/console/console.c -o $(BUILD)/console.o
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) $(BUILD)/multiboot_header.o $(BUILD)/entry.o $(BUILD)/kernel.o $(BUILD)/console.o -o $(KERNEL_ELF)

$(ISO): $(KERNEL_ELF) $(GRUB_CFG)
	rm -rf $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	cp $(GRUB_CFG) $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -m 512M

clean:
	rm -rf $(BUILD) $(ISO_DIR) $(ISO)
