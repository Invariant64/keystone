/*
 * lenet AI Model Test in Keystone Enclave
 * Based on resnet50-v1-7 template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ONNX2C generated functions (需要在模型转换完成后取消注释)
// extern void lenet_init(void);
// extern void lenet_run(float* input, float* output);
// extern void lenet_cleanup(void);

// Test data dimensions
#define INPUT_SIZE (3 * 64 * 64)  // 3 channels, 64x64 image
#define OUTPUT_SIZE 1000          // ImageNet classes

int main() {
    printf("🧠 lenet AI Model Test in Keystone Enclave\n");
    printf("===========================================\n\n");
    
    // 分配内存
    float* input = (float*)malloc(INPUT_SIZE * sizeof(float));
    float* output = (float*)malloc(OUTPUT_SIZE * sizeof(float));
    
    if (!input || !output) {
        printf("❌ Memory allocation failed!\n");
        return 1;
    }
    
    // 初始化测试数据
    printf("📊 Initializing test input (%d values)...\n", INPUT_SIZE);
    for (int i = 0; i < INPUT_SIZE; i++) {
        input[i] = (float)rand() / RAND_MAX;
    }
    
    printf("⚡ Running lenet inference in secure enclave...\n");
    clock_t start = clock();
    
    entry(input, output); // 调用 ONNX2C 生成的入口函数
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("🎯 Inference completed in %.3f seconds\n", time_taken);
    printf("📈 First 5 outputs: ");
    for (int i = 0; i < 5; i++) {
        printf("%.4f ", output[i]);
    }
    printf("\n");
    
    printf("✅ lenet enclave test completed successfully!\n");
    
    free(input);
    free(output);
    return 0;
}
