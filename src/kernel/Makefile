# MAKEFILE FOR i386 
# MAKES KERNEL

# $@ = target file
# $< = first dependency
# $^ = all dependencies

# detect all .o files based on their .c source
C_SOURCES = $(wildcard kernel/*.c drivers/*.c arch/i386/cpu/*.c)
HEADERS = $(wildcard kernel/*.h  drivers/*.h arch/i386/cpu/*.h)
OBJ_FILES = ${C_SOURCES:.c=.o arch/i386/cpu/interrupt.o}

CC ?= x86_64-elf-gcc
LD ?= x86_64-elf-ld

CFLAGS = -g -m32 -ffreestanding -fno-pie -fno-stack-protector -Iarch/i386/include -Iinclude

# First rule is the one executed when no parameters are fed to the Makefile
all: run

# Notice how dependencies are built as needed
kernel.bin: arch/i386/boot/kernel_entry.o ${OBJ_FILES}
	$(LD) -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

DustOS.bin: arch/i386/boot/mbr.bin kernel.bin
	cat $^ > $@

run: DustOS.bin
	qemu-system-i386 -fda $<

echo: DustOS.bin
	xxd $<

# only for debug
kernel.elf: arch/i386/boot/kernel_entry.o ${OBJ_FILES}
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
	$(RM) kernel/*.o
	$(RM) arch/i386/boot/*.o arch/i386/boot/*.bin
	$(RM) drivers/*.o
	$(RM) arch/i386/cpu/*.o
