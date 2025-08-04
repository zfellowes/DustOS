#include "console.h"
#include "paging_check.h"
#include <stdint.h>

void kernel_main()
{
	kclear();

	kprintln("DustOS v0.0.1");
	kprintln("[+] Kernel Loaded!");

	if (is_paging_enabled())
	{
		kprintf("[+] Paging enabled!");
	} else {
		kprintf("[-] Paging NOT enabled.");
	}

	for (;;) { __asm__ volatile ("hlt"); }
}
