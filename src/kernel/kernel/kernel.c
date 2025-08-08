#include "idt.h"
#include "isr.h"
#include "timer.h"
#include "display.h"
#include "keyboard.h"

#include "util.h"
#include "mem.h"

extern uint32_t tick;

void* alloc(int n) {
	int *ptr = (int *) mem_alloc(n * sizeof(int));
	    if (ptr == NULL_POINTER) {
	        kprint("[-] Memory not allocated.\n");
	    } else {
	        // Get the elements of the array
	        for (int i = 0; i < n; ++i) {
	//            ptr[i] = i + 1; // shorthand for *(ptr + i)
	        }
	
	        for (int i = 0; i < n; ++i) {
	//            char str[256];
	//            int_to_string(ptr[i], str);
	//            kprint(str);
	        }
	//        print_nl();
	    }
	return ptr;
}
	
void start_kernel() {
	clear_screen();
	kprint("[+] Installing interrupt service routines (ISRs).\n");
	isr_install();

	kprint("[+] Enabling external interrupts.\n");
	asm volatile("sti");

	kprint("[+] Initializing keyboard (IRQ 1).\n");
	init_keyboard();

	kprint("[+] Initializing dynamic memory.\n");
	init_dynamic_mem();

	kprint("[+] Initializing timer.\n");
	init_timer(1);

	clear_screen();
	kprint("DustOS v0.0.1\n");
	kprint("dust> ");
}

void execute_command(char *input) { //TODO create a command registry instead of an if statement for easier readability
	if (compare_string(input, "EXIT") == 0) {
		kprint("[!] Attempting Shutdown. Goodbye!\n");
		asm volatile (
			"mov $0x2000, %%eax\n\t" // shuts down qemu
			"mov $0x604, %%dx\n\t"
			"out %%ax, %%dx\n\t"
			"hlt\n\t" // fallback if it didn't work
			:
			:
			: "eax", "dx"
		);
	}
	else if (compare_string(input, "CLEAR") == 0) {
		clear_screen();
		kprint("dust> ");
	}
	else if (compare_string(input, "INFO") == 0) {
		kprint("[+] DustOS v0.0.1 - Kernel loaded\n");
		kprint("dust> ");
	}
	else if (compare_string(input, "DEBUG") == 0) {
		kprint("[*] x86 Kernel Debug:\n");
		//tick count
		char tick_str[32];
		int_to_string(tick, tick_str);
		kprint("[+] Ticks: ");
		kprint(tick_str);
		print_nl();
		// dynamic memory node size
		print_dynamic_node_size();
		// dynamic memory layout
		kprint("[+] Dynamic memory layout:\n");
		print_dynamic_mem();
		// total used memory
		print_dynamic_mem_summary();
		// cpu registers
		uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp;
		asm volatile("mov %%eax, %0" : "=r"(eax));
		asm volatile("mov %%ebx, %0" : "=r"(ebx));
		asm volatile("mov %%ecx, %0" : "=r"(ecx));
		asm volatile("mov %%edx, %0" : "=r"(edx));
		asm volatile("mov %%esi, %0" : "=r"(esi));
		asm volatile("mov %%edi, %0" : "=r"(edi));
		asm volatile("mov %%ebp, %0" : "=r"(ebp));
		asm volatile("mov %%esp, %0" : "=r"(esp));
		char reg_str[32];
		kprint("[+] CPU Registers:\n");

		int_to_string(eax, reg_str);
		kprint("EAX = "); kprint(reg_str); print_nl();

		int_to_string(ebx, reg_str);
		kprint("EBX = "); kprint(reg_str); print_nl();

		int_to_string(ecx, reg_str);
		kprint("ECX = "); kprint(reg_str); print_nl();

		int_to_string(edx, reg_str);
		kprint("EDX = "); kprint(reg_str); print_nl();

		int_to_string(esi, reg_str);
		kprint("ESI = "); kprint(reg_str); print_nl();

		int_to_string(edi, reg_str);
		kprint("EDI = "); kprint(reg_str); print_nl();

		int_to_string(ebp, reg_str);
		kprint("EBP = "); kprint(reg_str); print_nl();

		int_to_string(esp, reg_str);
		kprint("ESP = "); kprint(reg_str); print_nl();

		kprint("[+] End of debug dump.\n");
		kprint("dust> ");
	}
	else if (compare_string(input, "UPTIME") == 0) {
		char time_str[32];
		int_to_string(tick, time_str);
		kprint("[+] ");
		kprint(time_str);
		kprint("\ndust> ");
	}
	else if (starts_with(input, "ALLOC")) {
		int n = 0;
		char *arg = input + 6;
		if (*arg) {
			n = atoi(arg);
			if (n > 0) {
				alloc(n);
				kprint("[+] Allocation complete\n");
			} else {
				kprint("[-] Invalid size\n");
			}
		} else {
			kprint("[-] Usage: ALLOC <size>\n"); 
		}
		kprint("dust> ");
	}
	else if (compare_string(input, "HELP") == 0) {
		kprint("[+] Commands: EXIT, CLEAR, HELP, ECHO, INFO, UPTIME, DEBUG, ALLOC\ndust> ");
	}
	else if (starts_with(input, "ECHO")) {
		kprint(input + 5);
		kprint("\ndust> ");
	}
	else if (compare_string(input, "") == 0) {
		kprint("\ndust> ");
	} 
	else {
		kprint("[-] Unknown command: ");
		kprint(input);
		kprint("\ndust> ");
	}
}
