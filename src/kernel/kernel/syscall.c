#include <cpu/isr.h>
#include <kernel/kernel.h>

#define MAX_SYSCALLS 256
static syscall_t syscall_table[MAX_SYSCALLS];

void syscall_handler(registers_t *r)
{
	uint32_t num = r->eax;
	if (num < MAX_SYSCALLS && syscall_table[num]) {
		syscall_table[num]();
	} else {
		printk("[-] Unknown syscall: %d\n", num);
	}
}

void syscall_init()
{
	register_interrupt_handler(128, syscall_handler);
	printk("[+] Syscalls initialized (int 0x80)\n");
}

int register_syscall(uint32_t num, syscall_t func)
{
	if (num < MAX_SYSCALLS) {
		syscall_table[num] = func;
		return 0;
	}
	return -1;
}
