#pragma once
#include <stdint.h>
#include <cpu/isr.h>

typedef void (*syscall_t)(void);

void syscall_init();
void syscall_handler(registers_t *r);
int register_syscall(uint32_t num, syscall_t func);
