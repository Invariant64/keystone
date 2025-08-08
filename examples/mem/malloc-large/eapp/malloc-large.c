#include "keystone_memory_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>

#define K_NUM_BUFFERS     20        // 增加缓冲区数量
#define K_NUM_ITERATIONS  2000      // 增加迭代次数

// 小内存分配参数 (大部分时间使用)
#define K_SMALL_MIN_SIZE  (128 * 1024)     // 128KB
#define K_SMALL_MAX_SIZE  (500 * 1024)     // 500KB

// 大内存分配参数 (偶尔使用)
#define K_LARGE_MIN_SIZE  (5 * 1024 * 1024)   // 5MB
#define K_LARGE_MAX_SIZE  (50 * 1024 * 1024)  // 50MB

// 大内存分配概率 (1% 的概率分配大内存)
#define K_LARGE_ALLOC_PROBABILITY 1

static void *buffers[K_NUM_BUFFERS];

// 获取随机大小，根据概率选择小内存或大内存
static size_t rand_size() {
    int choice = rand() % 100;
    
    if (choice < K_LARGE_ALLOC_PROBABILITY) {
        // 1% 的概率分配大内存
        size_t size = K_LARGE_MIN_SIZE + rand() % (K_LARGE_MAX_SIZE - K_LARGE_MIN_SIZE + 1);
        printf("[LARGE] Allocating %zu bytes (%.2f MB)\n", size, size / (1024.0 * 1024.0));
        return size;
    } else {
        // 99% 的概率分配小内存
        return K_SMALL_MIN_SIZE + rand() % (K_SMALL_MAX_SIZE - K_SMALL_MIN_SIZE + 1);
    }
}

int main(void) {
    printf("=== Dynamic Memory Allocation Test ===\n");
    printf("Small allocations: %d KB - %d KB (99%% probability)\n", 
           K_SMALL_MIN_SIZE / 1024, K_SMALL_MAX_SIZE / 1024);
    printf("Large allocations: %d MB - %d MB (%d%% probability)\n", 
           K_LARGE_MIN_SIZE / (1024*1024), K_LARGE_MAX_SIZE / (1024*1024), K_LARGE_ALLOC_PROBABILITY);
    printf("Total iterations: %d\n", K_NUM_ITERATIONS);
    printf("==========================================\n");

    // 把 mmap 阈值改成 128 KiB
    if (mallopt(M_MMAP_THRESHOLD, 128 * 1024) == 1) {
        printf("M_MMAP_THRESHOLD set to 128 KiB\n");
    } else {
        printf("Failed to set M_MMAP_THRESHOLD\n");
    }
    
    srand(42); // 固定种子，保证可重复
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    print_current_memory_stats();
    
    int large_alloc_count = 0;
    int small_alloc_count = 0;
    
    for (int i = 0; i < K_NUM_ITERATIONS; i++) {
        int idx = rand() % K_NUM_BUFFERS;
        size_t sz = rand_size();

        // 统计分配类型
        if (sz >= K_LARGE_MIN_SIZE) {
            large_alloc_count++;
        } else {
            small_alloc_count++;
        }

        // 释放旧内存
        if (buffers[idx]) {
            free(buffers[idx]);
            buffers[idx] = NULL;
        }

        // 分配新内存
        buffers[idx] = malloc(sz);
        if (!buffers[idx]) {
            perror("malloc failed");
            return 1;
        }
        
        // 写一下触发真实分配
        ((char*)buffers[idx])[0] = 0xAA;
        if (sz > 1024) {
            ((char*)buffers[idx])[sz-1] = 0xBB; // 确保整个内存区域被使用
        }
        
        // 每100次迭代显示一次进度和内存统计
        if ((i + 1) % 100 == 0) {
            printf("Progress: %d/%d iterations completed\n", i + 1, K_NUM_ITERATIONS);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    long ms = (end.tv_sec - start.tv_sec) * 1000 +
              (end.tv_nsec - start.tv_nsec) / 1000000;
    long us_per_alloc = ((end.tv_sec - start.tv_sec) * 1000000 +
                        (end.tv_nsec - start.tv_nsec) / 1000) / K_NUM_ITERATIONS;

    printf("\n=== Test Results ===\n");
    printf("Total allocations: %d\n", K_NUM_ITERATIONS);
    printf("Small allocations: %d (%.1f%%)\n", small_alloc_count, 
           (float)small_alloc_count / K_NUM_ITERATIONS * 100);
    printf("Large allocations: %d (%.1f%%)\n", large_alloc_count, 
           (float)large_alloc_count / K_NUM_ITERATIONS * 100);
    printf("Execution time: %ld ms\n", ms);
    printf("Average time per allocation: %ld us\n", us_per_alloc);

    // 显示最终内存状态
    print_current_memory_stats();

    // 清理所有剩余内存
    printf("\nCleaning up allocated memory...\n");
    for (int i = 0; i < K_NUM_BUFFERS; i++) {
        if (buffers[i]) {
            free(buffers[i]);
            buffers[i] = NULL;
        }
    }
    
    // 显示清理后的内存状态
    print_current_memory_stats();

    return 0;
}
