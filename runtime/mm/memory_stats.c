#include "mm/common.h"
#include "mm/mm.h"
#include "mm/vm.h"
#include "mm/freemem.h"
#include "sys/timex.h"
#include "mm/vm_defs.h"
#include "sys/timex.h"

// 获取堆使用情况（包括 brk 和 mmap）
size_t get_heap_usage() {
    size_t brk_usage = 0;
    size_t mmap_usage = get_mmap_size(); // 获取 mmap 分配的内存
    
    uintptr_t current_break = get_program_break();
    
    // 堆起始地址是 EYRIE_ANON_REGION_START + 1GB
    // 这是在 boot.c 中设置的初始 program_break
    uintptr_t heap_start = EYRIE_ANON_REGION_START + (1024UL * 1024 * 1024);
    
    if (current_break > heap_start) {
        brk_usage = current_break - heap_start;
    }
    
    return brk_usage + mmap_usage; // 返回 brk + mmap 的总和
}
// 获取内存使用统计和时间
void get_memory_stats_with_time(size_t* heap_used, size_t* pages_available, size_t* total_memory, uint64_t* timestamp) {
    if (heap_used) {
        *heap_used = get_heap_usage();
    }
    if (pages_available) {
        *pages_available = spa_available() * RISCV_PAGE_SIZE;
    }
    if (total_memory) {
        *total_memory = freemem_size;
    }
    if (timestamp) {
        *timestamp = get_cycles64();
    }
}
