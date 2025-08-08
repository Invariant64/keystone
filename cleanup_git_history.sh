#!/bin/bash
# Git历史清理脚本 - 移除大文件

echo "🧹 Git历史清理脚本"
echo "=================="

# 备份当前分支
echo "📋 备份当前分支..."
git branch backup-before-cleanup

# 使用git filter-repo清理大文件（如果安装了的话）
if command -v git-filter-repo >/dev/null 2>&1; then
    echo "🔧 使用git-filter-repo清理..."
    git filter-repo --path-glob 'examples/ML/wideresnet/eapp/wideresnet.c' --invert-paths
    git filter-repo --path-glob 'examples/simple-neural-network-read-write/eapp/data/train-images-idx3-ubyte.h' --invert-paths
    git filter-repo --path-glob 'examples/ML/densenet/eapp/densenet.c' --invert-paths --force
    git filter-repo --path-glob 'examples/ML/resnet50/eapp/resnet50.c' --invert-paths --force
    git filter-repo --path-glob 'examples/ML/resnext29/eapp/resnext29.c' --invert-paths --force
else
    echo "❌ git-filter-repo 未安装"
    echo "💡 建议："
    echo "1. 安装: pip install git-filter-repo"
    echo "2. 或者使用强制推送覆盖远程分支（危险）"
    echo "3. 或者创建新分支重新提交干净版本"
fi

echo "✅ 清理完成！"
