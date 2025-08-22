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
#include <kernel/cmd.h>

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
	
	// Initialize command registry
	cmd_init();
	
	print_string("dust> ");
}

void execute_command(char *input) {
	// Use the new command registry system
	cmd_execute(input);
}