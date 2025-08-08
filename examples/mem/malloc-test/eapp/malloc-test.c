#include "keystone_memory_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define MAX_TOTAL_ALLOC (64 * 1024 * 1024) // 64MB
#define MIN_ALLOC_SIZE  (1 * 1024 * 1024)  // 1MB
#define MAX_ALLOC_SIZE  (64 * 1024 * 1024) // 64MB
#define MAX_BLOCKS      10000              // 最多保留这么多块指针

typedef struct {
    void* ptr;
    size_t size;
} Block;

int main(int argc, char* argv[]) {
    Block blocks[MAX_BLOCKS];
    size_t block_count = 0;
    size_t total_allocated = 0;

    srand(time(NULL));
    int iterations = 1000;
    if (argc > 1) {
        iterations = atoi(argv[1]);
    }

    for (int i = 0; i < iterations; i++) {
        // 随机选择操作：0 = 分配，1 = 释放（优先分配）
        int do_alloc = (rand() % 100 < 70 || block_count == 0); // 70% 概率尝试分配

        if (do_alloc) {
            size_t size = (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE + 1)) + MIN_ALLOC_SIZE;

            if (total_allocated + size <= MAX_TOTAL_ALLOC) {
                void* ptr = malloc(size);
                if (!ptr) {
                    perror("malloc failed");
                    continue;
                }
                blocks[block_count++] = (Block){.ptr = ptr, .size = size};
                total_allocated += size;
                printf("[ALLOC] %zu bytes | Total = %zu bytes | Blocks = %zu\n", size, total_allocated, block_count);
            } else {
                // 超出限制，强制释放一个块
                size_t victim = rand() % block_count;
                free(blocks[victim].ptr);
                total_allocated -= blocks[victim].size;
                printf("[FREE*] %zu bytes | Total = %zu bytes | Blocks = %zu\n", blocks[victim].size, total_allocated, block_count - 1);
                blocks[victim] = blocks[--block_count];
            }
        } else {
            // 主动释放一个块
            size_t victim = rand() % block_count;
            free(blocks[victim].ptr);
            total_allocated -= blocks[victim].size;
            printf("[FREE ] %zu bytes | Total = %zu bytes | Blocks = %zu\n", blocks[victim].size, total_allocated, block_count - 1);
            blocks[victim] = blocks[--block_count];
        }
    }

    // 清理剩余内存
    for (size_t i = 0; i < block_count; i++) {
        free(blocks[i].ptr);
    }

    printf("Done. All memory freed.\n");
    return 0;
}
