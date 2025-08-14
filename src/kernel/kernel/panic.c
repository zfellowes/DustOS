#include <stdbool.h>
#include <stdint.h>
#include "../drivers/display.h"
#include "util.h"

void panic(const char* message)
{
	print_string("[-] KERNEL PANIC: ");
	print_string(message);
	print_nl();
	for(;;);
}
