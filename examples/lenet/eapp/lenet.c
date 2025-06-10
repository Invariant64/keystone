#include "THGeneral.h"

// 在包含其他头文件之前设置TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/THTensor.h"

#define TH_REAL_IS_FLOAT
#define Real Float
#define real float

#include "THTensor.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== TH Library Simple Test ===\n");
    
    // 现在THTensor应该正确展开为THFloatTensor
    THTensor *tensor = THTensor_(newWithSize2d)(3, 3);
    if (tensor) {
        printf("✓ Successfully created 3x3 tensor\n");
        printf("  Tensor size: [%ld, %ld]\n", 
               THTensor_(size)(tensor, 0), 
               THTensor_(size)(tensor, 1));
        printf("  Total elements: %ld\n", THTensor_(nElement)(tensor));
        
        // 填充数据
        THTensor_(fill)(tensor, 2.5);
        real *data = THTensor_(data)(tensor);
        printf("  First element: %.2f\n", data[0]);
        
        THTensor_(free)(tensor);
        printf("✓ Test completed successfully\n");
    } else {
        printf("✗ Failed to create tensor\n");
        return 1;
    }
    
    return 0;
}