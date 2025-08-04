#include "paging_check.h"

/* sample paging check for 64 bit x86_64. dustos does not support x86_64 currently so it is commented out
#if defined(__x86_64__) 

static inline uint64_t read_cr0(void)
{
	uint64_t v;
	__asm__ volatile("mov %%cr0, %0" : "=r"(v));
	return v;
}

static inline uint64_t read_cr3(void)
{
	uint64_t v;
	__asm__ volatile("mov %%cr3, %0" : "=r"(v));
	return v;
}

bool is_paging_enabled(void)
{
	uint64_t cr0 = read_cr0();
	return !!(cr0 & (1ULL << 31));
}

uint64_t get_page_table_base(void)
{
	return read_cr3();
}

*/

#if defined(__i386__)

static inline uint32_t read_cr0(void)
{
	uint32_t v;
	__asm__ volatile("mov %%cr0, %0" : "=r"(v));
	return v;
}

static inline uint32_t read_cr3(void)
{
	uint32_t v;
	__asm__ volatile("mov %%cr3, %0" : "=r"(v));
	return v;
}

bool is_paging_enabled(void)
{
	uint32_t cr0 = read_cr0();
	return !!(cr0 & (1u << 31));
}

uint64_t get_page_table_base(void)
{
	return (uint64_t)read_cr3();
}

#else

// Fallback for unsupported architectures
bool is_paging_enabled(void)
{
	return false;
}

uint64_t get_page_table_base(void)
{
	return 0;
}

#endif
