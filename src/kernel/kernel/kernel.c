#include <cpu/idt.h>
#include <cpu/isr.h>
#include <cpu/timer.h>
#include <drivers/display.h>
#include <drivers/keyboard.h>
#include <kernel/util.h>
#include <kernel/mem.h>
#include <kernel/panic.h>
#include <kernel/printk.h>
#include <kernel/syscall.h>

extern uint32_t tick;

void trigger_syscall()
{
	asm volatile(
		"mov $0, %%eax\n\t"
		"int $0x80"
		:
		:
		: "eax"
	);
}

void test_syscall()
{
	print_string("[+] Syscall executed!\n");
}

void* alloc(int n) {
	int *ptr = (int *) mem_alloc(n * sizeof(int));
	    if (ptr == NULL_POINTER) {
	        print_string("[-] Memory not allocated.\n");
	    } else {
	        // Get the elements of the array
	        for (int i = 0; i < n; ++i) {
	//            ptr[i] = i + 1; // shorthand for *(ptr + i)
	        }
	
	        for (int i = 0; i < n; ++i) {
	//            char str[256];
	//            int_to_string(ptr[i], str);
	//            print_string(str);
	        }
	//        print_nl();
	    }
	return ptr;
}

void start_kernel() {
	clear_screen();
	print_string("[+] Installing interrupt service routines (ISRs).\n");
	isr_install();

	print_string("[+] Installing syscalls.\n");
	syscall_init();

	print_string("[+] Enabling external interrupts.\n");
	asm volatile("sti");

	print_string("[+] Initializing keyboard (IRQ 1).\n");
	init_keyboard();

	print_string("[+] Initializing dynamic memory.\n");
	init_dynamic_mem();

	print_string("[+] Initializing timer.\n");
	init_timer(1);

	clear_screen();

	print_string("DustOS v0.0.1\n");
	print_string("dust> ");
}





void execute_command(char *input) { //TODO create a command registry instead of an if statement for easier readability
	if (compare_string(input, "EXIT") == 0) {
		print_string("[!] Attempting Shutdown. Goodbye!\n");
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
	else if (compare_string(input, "PANIC") == 0) {
		panic("DEBUG");
	}
	else if (compare_string(input, "CLEAR") == 0) {
		clear_screen();
		print_string("dust> ");
	}
	else if (compare_string(input, "INFO") == 0) {
		print_string("[+] DustOS v0.0.1 - Kernel loaded\n");
		print_string("dust> ");
	}
	else if (compare_string(input, "DEBUG") == 0) {
		print_string("[*] x86 Kernel Debug:\n");
		//tick count
		char tick_str[32];
		int_to_string(tick, tick_str);
		print_string("[+] Ticks: ");
		print_string(tick_str);
		print_nl();
		// dynamic memory node size
		print_dynamic_node_size();
		// dynamic memory layout
		print_string("[+] Dynamic memory layout:\n");
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
		print_string("[+] CPU Registers:\n");

		int_to_string(eax, reg_str);
		print_string("EAX = "); print_string(reg_str); print_nl();

		int_to_string(ebx, reg_str);
		print_string("EBX = "); print_string(reg_str); print_nl();

		int_to_string(ecx, reg_str);
		print_string("ECX = "); print_string(reg_str); print_nl();

		int_to_string(edx, reg_str);
		print_string("EDX = "); print_string(reg_str); print_nl();

		int_to_string(esi, reg_str);
		print_string("ESI = "); print_string(reg_str); print_nl();

		int_to_string(edi, reg_str);
		print_string("EDI = "); print_string(reg_str); print_nl();

		int_to_string(ebp, reg_str);
		print_string("EBP = "); print_string(reg_str); print_nl();

		int_to_string(esp, reg_str);
		print_string("ESP = "); print_string(reg_str); print_nl();

		register_syscall(0, test_syscall);
		trigger_syscall();

		print_string("[+] End of debug dump.\n");
		print_string("dust> ");
	}
	else if (compare_string(input, "UPTIME") == 0) {
		char time_str[32];
		int_to_string(tick, time_str);
		print_string("[+] ");
		print_string(time_str);
		print_string("\ndust> ");
	}
	else if (starts_with(input, "ALLOC")) {
		int n = 0;
		char *arg = input + 6;
		if (*arg) {
			n = atoi(arg);
			if (n > 0) {
				alloc(n);
				print_string("[+] Allocation complete\n");
			} else {
				print_string("[-] Invalid size\n");
			}
		} else {
			print_string("[-] Usage: ALLOC <size>\n"); 
		}
		print_string("dust> ");
	}
	else if (compare_string(input, "HELP") == 0) {
		print_string("[+] Commands: EXIT, CLEAR, HELP, ECHO, INFO, UPTIME, DEBUG, ALLOC, PANIC\ndust> ");
	}
	else if (starts_with(input, "ECHO")) {
		print_string(input + 5);
		print_string("\ndust> ");
	}
	else if (compare_string(input, "") == 0) {
		print_string("\ndust> ");
	} 
	else {
		print_string("[-] Unknown command: ");
		print_string(input);
		print_string("\ndust> ");
	}
}
