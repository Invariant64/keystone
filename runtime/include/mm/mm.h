#ifndef _MM_H_
#define _MM_H_
#include <stddef.h>
#include <stdint.h>

#include "mm/vm_defs.h"

uintptr_t translate(uintptr_t va);
pte* pte_of_va(uintptr_t va);
uintptr_t map_page(uintptr_t vpn, uintptr_t ppn, int flags);
uintptr_t alloc_page(uintptr_t vpn, int flags);
uintptr_t realloc_page(uintptr_t vpn, int flags);
void free_page(uintptr_t vpn);
size_t alloc_pages(uintptr_t vpn, size_t count, int flags);
void free_pages(uintptr_t vpn, size_t count);
size_t test_va_range(uintptr_t vpn, size_t count);

uintptr_t get_program_break();
void set_program_break(uintptr_t new_break);

void map_with_reserved_page_table(uintptr_t base, uintptr_t size, uintptr_t ptr, pte* l2_pt, pte* l3_pt);

// Memory statistics functions
size_t get_heap_usage();
void get_memory_stats_with_time(size_t* heap_used, size_t* pages_available, size_t* total_memory, uint64_t* timestamp);

// mmap memory tracking functions
void add_mmap_size(size_t size);
void remove_mmap_size(size_t size);
size_t get_mmap_size();

#endif /* _MM_H_ */
