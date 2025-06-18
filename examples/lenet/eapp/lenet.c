#define TH_REAL_IS_FLOAT
#include <TH.h>

#include <stdio.h>

int main() {
    printf("=== TH Library Simple Test ===\n");
    
    // 现在THTensor应该正确展开为THFloatTensor
    THFloatTensor *tensor = THFloatTensor_newWithSize2d(3, 3);
    if (tensor) {
        printf("✓ Successfully created 3x3 tensor\n");
        printf("  Tensor size: [%ld, %ld]\n", 
               THFloatTensor_size(tensor, 0), 
               THFloatTensor_size(tensor, 1));
        printf("  Total elements: %ld\n", THFloatTensor_nElement(tensor));
        
        // 填充数据
        THFloatTensor_fill(tensor, 2.5);
        float *data = THFloatTensor_data(tensor);
        printf("  First element: %.2f\n", data[0]);
        
        THFloatTensor_free(tensor);
        printf("✓ Test completed successfully\n");
    } else {
        printf("✗ Failed to create tensor\n");
        return 1;
    }
    
    return 0;
}