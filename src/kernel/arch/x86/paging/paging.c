#include "paging.h"
#include <stddef.h>
#include <stdint.h>

__attribute__((aligned(4096))) static uint32_t page_directory[1024];
__attribute__((aligned(4096))) static uint32_t first_page_table[1024];

static inline void load_cr3(uint32_t val)
{
	__asm__ volatile ("mov %0, %%cr3" :: "r"(val));
}

static inline uint32_t read_cr0(void)
{
	uint32_t val;
	__asm__ volatile ("mov %%cr0, %0" : "=r"(val));
	return val;
}

static inline void write_cr0(uint32_t val)
{
	__asm__ volatile ("mov %0, %%cr0" :: "r"(val));
}

void init_paging(void)
{
	for (size_t i = 0; i < 1024; ++i) {
		first_page_table[i] = 0;
		page_directory[i] = 0;
	}

	for (size_t i = 0; i < 1024; ++i) {
		uint32_t phys = (uint32_t)(i * 0x1000);
		first_page_table[i] = phys | PAGE_PRESENT | PAGE_RW;
	}

	page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_RW;

	load_cr3((uint32_t)page_directory);

	uint32_t cr0 = read_cr0();
	cr0 |= 0x80000000;
	write_cr0(cr0);
}
