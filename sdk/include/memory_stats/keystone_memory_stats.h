#ifndef KEYSTONE_MEMORY_STATS_H
#define KEYSTONE_MEMORY_STATS_H

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// ========== 系统调用号 ==========
#define SYS_keystone_get_memory_stats 500

// ========== 内存统计结构体 ==========
struct keystone_memory_stats {
    size_t heap_used;        // 已使用堆空间（字节）
    size_t pages_available;  // 可用物理内存（字节）
    size_t total_memory;     // 总内存（字节）
    uint64_t timestamp;      // 时间戳
};

// ========== 获取内存状态 ==========
static inline int get_memory_stats_with_time(struct keystone_memory_stats* stats) {
    if (!stats) return -1;

    return syscall(SYS_keystone_get_memory_stats, 
                   &stats->heap_used, 
                   &stats->pages_available, 
                   &stats->total_memory, 
                   &stats->timestamp);
}

// ========== 打印内存状态 ==========
// 上次打印的内存利用率
static double last_printed_utilization = 0;
// 第一次记录的总内存大小，用于后续计算
static size_t first_total_memory = 0;

static inline void print_memory_usage(const struct keystone_memory_stats* stats) {
    if (!stats) return;

    // 记录第一次的总内存大小
    if (first_total_memory == 0 && stats->total_memory > 0) {
        first_total_memory = stats->total_memory;
    }

    // 使用第一次记录的总内存大小来计算内存利用率
    double current_utilization = 0.0;
    if (first_total_memory > 0) {
        current_utilization = (double)stats->heap_used / first_total_memory * 100;
    }

    // 检查是否需要打印（第一次打印或变化超过1%）
    bool should_print = false;
    if (last_printed_utilization < 0) {
        // 第一次打印
        should_print = true;
    } else {
        // 检查变化是否超过0%
        double change = current_utilization - last_printed_utilization;
        if (change < 0) change = -change; // 取绝对值
        if (change > 0) {
            should_print = true;
        }
    }

    if (should_print) {
        printf("=== Memory Statistics ===\n");
        printf("Heap used:        %zu bytes (%.2f KB)\n", 
               stats->heap_used, stats->heap_used / 1024.0);
        printf("Available memory: %zu bytes (%.2f KB)\n", 
               stats->pages_available, stats->pages_available / 1024.0);
        printf("Total memory:     %zu bytes (%.2f KB)\n", 
               first_total_memory, first_total_memory / 1024.0);
        printf("Memory utilization: %.2f%%\n", current_utilization);
        printf("Timestamp:        %lu cycles\n", stats->timestamp);
        printf("========================\n");
        
        // 更新上次打印的利用率
        last_printed_utilization = current_utilization;
    }
}

#ifdef KEYSTONE_ENABLE_TRACKING

// ========== 开启内存追踪 ==========

// 全局统计结构
static struct keystone_memory_stats g_memory_stats = {0};

// 替代 malloc
static inline void* tracked_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr && get_memory_stats_with_time(&g_memory_stats) == 0) {
        print_memory_usage(&g_memory_stats);
    }
    return ptr;
}

// 替代 free
static inline void tracked_free(void* ptr) {
    if (ptr) {
        free(ptr);
        if (get_memory_stats_with_time(&g_memory_stats) == 0) {
            print_memory_usage(&g_memory_stats);
        }
    }
}

// 替代 calloc
static inline void* tracked_calloc(size_t num, size_t size) {
    void* ptr = calloc(num, size);
    if (ptr && get_memory_stats_with_time(&g_memory_stats) == 0) {
        print_memory_usage(&g_memory_stats);
    }
    return ptr;
}

// 替代 realloc
static inline void* tracked_realloc(void* ptr, size_t size) {
    void* old_ptr = ptr;
    void* new_ptr = realloc(ptr, size);
    if ((new_ptr || size == 0) && get_memory_stats_with_time(&g_memory_stats) == 0) {
        print_memory_usage(&g_memory_stats);
    }
    return new_ptr;
}

// 宏替换 malloc/free 系列
#define malloc(size)        tracked_malloc(size)
#define free(ptr)           tracked_free(ptr)
#define calloc(num, size)   tracked_calloc(num, size)
#define realloc(ptr, size)  tracked_realloc(ptr, size)

// 手动打印当前状态
static inline void print_current_memory_stats(void) {
    if (get_memory_stats_with_time(&g_memory_stats) == 0) {
        print_memory_usage(&g_memory_stats);
    } else {
        printf("Failed to get memory statistics\n");
    }
}

#else

// ===== 未启用 KEYSTONE_ENABLE_TRACKING：空定义 =====
#define print_current_memory_stats() do { } while (0)

#endif  // KEYSTONE_ENABLE_TRACKING

#endif  // KEYSTONE_MEMORY_STATS_H
