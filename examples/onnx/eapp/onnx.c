#include <stdio.h>
// #include "lenet.c"  // 假设你生成的代码是 lenet.c

int main() {
    // 1. 输入图像（模拟一个手写数字图像）
    float input[1][1][28][28] = {{{{0}}}};
    // 假设你自己用图片处理后填进去。这里全为0只是为了测试。

    // 2. 输出缓冲区
    float output[1][10][1][1] = {0};

    // 3. 调用推理函数
    entry(input, output);

    // 4. 打印输出（logits）
    printf("Output:\n");
    for (int i = 0; i < 10; ++i) {
        printf("Class %d: %f\n", i, output[0][i][0][0]);
    }

    return 0;
}
