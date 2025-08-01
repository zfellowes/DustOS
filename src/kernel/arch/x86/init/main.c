#include "../kernel/console/console.h"

void kernel_main()
{
	kclear();
	kprintln("DustOS v0.0.1");
	kprintln("[+] Kernel Loaded!");


	for (;;) {
		__asm__ volatile ("hlt");
	}
}
