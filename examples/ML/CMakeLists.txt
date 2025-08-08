cmake_minimum_required(VERSION 3.10)

# Machine Learning Models
# =======================
# This directory contains AI/ML model examples for Keystone enclave

# 目前没有注释的是可以编译成功并运行的

# Neural Network Models
add_subdirectory(lenet)
add_subdirectory(squeezenet)

# ResNet Family
add_subdirectory(resnet50)
# add_subdirectory(resnet110_no_pretrain)

# # ResNeXt Family
add_subdirectory(resnext29)

# # Other Architectures
# add_subdirectory(inceptionv3)
# add_subdirectory(vgg19_compatible)
add_subdirectory(densenet)
# add_subdirectory(wideresnet) #太大了

# # Legacy/Test versions
# add_subdirectory(lenet-TH)
