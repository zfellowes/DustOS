#pragma once
#include <stdint.h>
#include <stddef.h>

#define PAGE_PRESENT 0x1
#define PAGE_RW 0x2
#define PAGE_USER 0x4

#define KERNEL_VIRT_BASE 0xC0000000U
void init_paging(void);

