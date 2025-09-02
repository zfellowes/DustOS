# MAKEFILE FOR i386 

# $@ = target file
# $< = first dependency
# $^ = all dependencies

# Kernel Sources
C_SOURCES = $(wildcard src/kernel/kernel/*.c src/kernel/drivers/*.c src/kernel/arch/i386/cpu/*.c)
ASM_SOURCES = $(wildcard src/kernel/arch/i386/cpu/*.asm)
HEADERS = $(wildcard src/kernel/include/kernel/*.h  src/kernel/include/drivers/*.h src/kernel/arch/i386/include/cpu/*.h)

OBJ_FILES = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o}

CC ?= i386-elf-gcc
LD ?= i386-elf-ld

CFLAGS = -g -m32 -ffreestanding -fno-pie -fno-stack-protector -Isrc/kernel/arch/i386/include -Isrc/kernel/include

# First rule is the one executed when no parameters are fed to the Makefile
all: run

# Notice how dependencies are built as needed
kernel.bin: src/bootloader/arch/i386/kernel_entry.o ${OBJ_FILES}
	$(LD) -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

DustOS.bin: src/bootloader/arch/i386/mbr.bin kernel.bin
	cat $^ > $@

run: DustOS.bin
	qemu-system-i386 -fda $<

echo: DustOS.bin
	xxd $<

# only for debug
kernel.elf: src/bootloader/arch/i386/kernel_entry.o ${OBJ_FILES}
	$(LD) -m elf_i386 -o $@ -Ttext 0x1000 $^

debug: DustOS.bin kernel.elf
	qemu-system-i386 -s -S -fda DustOS.bin -d guest_errors,int &
	i386-elf-gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	$(CC) $(CFLAGS) -c $< -o $@ 

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

%.dis: %.bin
	ndisasm -b 32 $< > $@

clean:
	$(RM) *.bin *.o *.dis *.elf
	$(RM) src/kernel/kernel/*.o
	$(RM) src/bootloader/arch/i386/*.o src/bootloader/arch/i386/*.bin
	$(RM) src/kernel/drivers/*.o
	$(RM) src/kernel/arch/i386/cpu/*.o
