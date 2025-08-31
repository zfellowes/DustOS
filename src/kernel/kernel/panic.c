#include <stdbool.h>
#include <stdint.h>
#include <drivers/display.h>
#include <kernel/kernel.h>

void panic(const char* message)
{
	asm volatile("cli"); // disable interrupts
	print_string("[-] KERNEL PANIC: ");
	print_string(message);
	print_nl();
	for(;;);
}
