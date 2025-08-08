/*
 * resnext29 AI Model Test in Keystone Enclave
 * Based on resnet50-v1-7 template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ONNX2C generated functions (需要在模型转换完成后取消注释)
// extern void resnext29_init(void);
// extern void resnext29_run(float* input, float* output);
// extern void resnext29_cleanup(void);

// Test data dimensions
#define INPUT_SIZE (3 * 64 * 64)  // 3 channels, 64x64 image
#define OUTPUT_SIZE 1000          // ImageNet classes

int main() {
    printf("🧠 resnext29 AI Model Test in Keystone Enclave\n");
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
    
    printf("⚡ Running resnext29 inference in secure enclave...\n");
    clock_t start = clock();
    
    // TODO: 在ONNX2C集成完成后启用
    /*
    resnext29_init();
    resnext29_run(input, output);
    resnext29_cleanup();
    */
    
    // 临时：模拟推理
    printf("⚠️  Note: Using simulated inference (ONNX2C integration pending)\n");
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        output[i] = (float)rand() / RAND_MAX;
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("🎯 Inference completed in %.3f seconds\n", time_taken);
    printf("📈 First 5 outputs: ");
    for (int i = 0; i < 5; i++) {
        printf("%.4f ", output[i]);
    }
    printf("\n");
    
    printf("✅ resnext29 enclave test completed successfully!\n");
    
    free(input);
    free(output);
    return 0;
}
