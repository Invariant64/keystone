# Keystone Examples Directory Structure

本目录包含了各种Keystone enclave示例程序，按功能分类组织。

## 📂 目录结构

### 🧠 ML/ - 机器学习模型
包含14个不同的AI/ML模型，已转换为C代码用于enclave测试：
- ResNet系列 (resnet50, resnet110_no_pretrain, wideresnet等)
- ResNeXt系列 (resnext29, resnext50变体等) 
- CNN经典架构 (vgg19_compatible, inceptionv3, densenet)
- 轻量级模型 (squeezenet, lenet)

### ⚡ rv8-bench/ - RV8基准测试套件
来自rv8项目的RISC-V基准测试：
- **加密算法**: aes, sha512, norx
- **系统基准**: dhrystone, primes  
- **数据处理**: bigint, miniz, qsort

### 💾 mem/ - 内存测试套件
内存相关的基准测试和工具：
- **内存基准**: stream, membench
- **分配测试**: malloc-test, malloc-large
- **监控工具**: memory_stats

### 🔧 其他示例
- **基础示例**: hello, hello-native, hello-cpp
- **安全功能**: attestation
- **I/O测试**: fileio, iozone
- **性能基准**: coremark, cc, gcbench
- **压力测试**: stress_test, tests

## 🚀 使用方法

### 构建所有示例
```bash
# 在keystone根目录
mkdir build && cd build
cmake .. -DKEYSTONE_SDK_DIR=/path/to/keystone/sdk
make examples
```

### 构建特定分类
```bash
make ML          # 构建所有机器学习模型
make rv8-bench   # 构建所有rv8基准测试
make memory      # 构建所有内存测试
```

### 构建单个示例
```bash
make lenet       # ML目录下的LeNet模型
make dhrystone   # rv8-bench目录下的Dhrystone基准
make stream      # mem目录下的Stream内存基准
```

## 📋 模型说明

### AI模型特点
- **输入尺寸**: 优化为1×3×64×64 (适合enclave内存限制)
- **格式**: ONNX转C代码，无外部依赖
- **用途**: 内存使用测试、性能基准、安全评估

### 基准测试特点
- **rv8-bench**: 标准RISC-V基准测试套件
- **mem**: 专注内存性能和分配模式测试
- **兼容性**: 完全适配Keystone enclave环境

## 🔬 开发指南

添加新示例时，请按功能分类放入相应目录，并更新对应的CMakeLists.txt文件。
