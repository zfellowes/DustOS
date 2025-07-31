NASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld

CFLAGS = -m32 -ffreestanding -O0 -Wall -Wextra
LDFLAGS = -m elf_i386

BUILD = build
ISO = DustOS.img

BOOTLOADER_SRC = src/bootloader/bootloader.s
KERNEL_ASM_SRC = src/kernel/entry.s
KERNEL_C_SRC = src/kernel/kernel.c
LINKER_SCRIPT = src/linkers/kernel.ld

BOOTLOADER_BIN = $(BUILD)/bootloader.bin
KERNEL_OBJ = $(BUILD)/kernel.o
ENTRY_OBJ = $(BUILD)/entry.o
KERNEL_BIN = $(BUILD)/kernel.bin

all: run

$(BOOTLOADER_BIN): $(BOOTLOADER_SRC)
	mkdir -p $(BUILD)
	$(NASM) -f bin $< -o $@

$(KERNEL_OBJ): $(KERNEL_C_SRC)
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(ENTRY_OBJ): $(KERNEL_ASM_SRC)
	mkdir -p $(BUILD)
	$(NASM) -f elf32 $< -o $@

$(KERNEL_BIN): $(ENTRY_OBJ) $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) --oformat binary -o $@ $^

$(ISO): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	cp $(BOOTLOADER_BIN) $@
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc

run: $(ISO)
	qemu-system-i386 -hda $(ISO) -m 256M

clean:
	rm -rf $(BUILD) $(ISO)

.PHONY: all clean run

