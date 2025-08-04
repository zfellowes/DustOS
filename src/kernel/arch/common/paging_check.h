#pragma once
#include <stdint.h>
#include <stdbool.h>

bool is_paging_enabled(void);
uint64_t get_page_table_base(void);

