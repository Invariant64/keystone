# Machine Learning Models for Keystone

本目录包含5个AI/ML模型的Keystone enclave示例：

## 📋 支持的模型
- **lenet**: 经典的LeNet卷积神经网络
- **squeezenet**: 轻量级CNN架构  
- **resnet50**: ResNet-50深度残差网络
- **resnext29**: ResNeXt-29网络
- **densenet**: DenseNet密集连接网络

## 🔧 构建说明

### 1. 生成模型C代码
由于生成的.c文件太大（>100MB），无法提交到Git，需要本地生成：

```bash
# 在test/ai目录中转换ONNX到C
cd ../../test/ai
make lenet squeezenet resnet50 resnext29 densenet

# 复制.c文件到ML目录
cp lenet/lenet.c ../../examples/ML/lenet/eapp/
cp squeezenet/squeezenet.c ../../examples/ML/squeezenet/eapp/
cp resnet50/resnet50.c ../../examples/ML/resnet50/eapp/
cp resnext29/resnext29.c ../../examples/ML/resnext29/eapp/
cp densenet/densenet.c ../../examples/ML/densenet/eapp/
```

### 2. 构建ML模型
```bash
# 在build目录中
make ML
```

### 3. 运行模型
```bash
# 运行单个模型
./lenet.ke
./squeezenet.ke
# 等等...
```

## ⚠️ 注意事项
- 模型.c文件通过ONNX2C从.onnx文件生成
- 生成的.c文件很大，不适合Git管理
- 输入尺寸优化为64×64以适配enclave内存限制
- Host端内存设置为512MB以支持大模型
