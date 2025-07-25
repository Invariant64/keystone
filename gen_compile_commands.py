#!/usr/bin/env python3
import os
import json
import shutil
import subprocess
from pathlib import Path
import re
import sys

# 当前工作目录作为根路径
SOURCE_DIR = Path.cwd()
BUILDROOT_BUILD_DIR = SOURCE_DIR / "build-generic64" / "buildroot.build"
FINAL_DB = SOURCE_DIR / "compile_commands.json"

# Bear 拦截的 Buildroot 模块
BEAR_MODULES = [
    "host-keystone-sdk",     # 必须最先构建
    "keystone-driver",       # 提供 keystone 设备接口
    "keystone-sm",           # sm 依赖 driver，也可能引用 sdk
    "keystone-runtime",      # runtime 依赖 sdk 和 sm
    "keystone-examples",     # examples 依赖 runtime、sm 和 sdk
]
# 映射构建产物路径 -> 源代码路径
MODULE_REMAP = {
    "keystone-examples": "examples",
    "keystone-runtime": "runtime",
    "keystone-sm": "sm",
    "keystone-driver": "linux-keystone-driver",
    "opensbi": "opensbi",
    "host-keystone-sdk": "sdk",
}

def run(cmd, cwd=None):
    print(f"\n▶️ 执行: {cmd}")
    subprocess.run(cmd, shell=True, cwd=cwd, check=True)

def build_with_bear(module):
    print(f"\n🔁 使用 bear 构建模块: {module}")
    run(f"BUILDROOT_TARGET={module}-dirclean make -j", cwd=SOURCE_DIR)
    run(f"bear -- make -j BUILDROOT_TARGET={module}", cwd=SOURCE_DIR)
    shutil.move("compile_commands.json", f"compile_commands_{module}.json")

def generate_sdk_compile_commands():
    print("\n📦 生成 SDK 的伪编译命令")
    sdk_dir = SOURCE_DIR / "sdk"
    cpp_files = list((sdk_dir / "src").rglob("*.cpp"))
    entries = []

    for f in cpp_files:
        entries.append({
            "directory": str(sdk_dir),
            "file": str(f),
            "arguments": [
                "riscv64-unknown-linux-gnu-g++",
                "-I", str(sdk_dir / "include"),
                "-I", str(sdk_dir / "src"),
                "-std=c++17",
                "-c", str(f),
                "-o", "/dev/null"
            ]
        })

    with open("compile_commands_sdk.json", "w") as f:
        json.dump(entries, f, indent=2)
    print(f"✅ 写入 compile_commands_sdk.json（共 {len(entries)} 项）")

def build_sdk_with_bear():
    print("\n📦 用 bear 构建 SDK")
    sdk_dir = SOURCE_DIR / "sdk"
    build_dir = sdk_dir / "build"
    os.makedirs(build_dir, exist_ok=True)
    cpp_files = list((sdk_dir / "src").rglob("*.cpp"))
    cmds = [
        f"riscv64-linux-gnu-g++ -I../include -I../src -std=c++17 -c {cpp} -o {build_dir/cpp.name}.o"
        for cpp in cpp_files
    ]
    script_path = sdk_dir / "build_sdk.sh"
    with open(script_path, "w") as f:
        f.write("#!/bin/bash\nset -e\n" + "\n".join(cmds))
    script_path.chmod(0o755)
    run(f"bear -- {script_path}", cwd=sdk_dir)
    shutil.move(sdk_dir / "compile_commands.json", "compile_commands_sdk.json")


def map_to_source(file_path: Path):
    try:
        relative = file_path.relative_to(BUILDROOT_BUILD_DIR)
        parts = list(relative.parts)

        # Buildroot 构建路径通常是 build/<module-hash>/
        if len(parts) > 1:
            match = re.match(r"(keystone-[a-z]+|opensbi)-[a-f0-9]+", parts[1])
            if match:
                module_prefix = match.group(1)
                if module_prefix in MODULE_REMAP:
                    parts[1] = MODULE_REMAP[module_prefix]
                    parts.pop(0)  # 移除 build/
                    new_path = SOURCE_DIR.joinpath(*parts)
                    if new_path.exists():
                        print(f"🔄 修复路径: {file_path} -> {new_path}")
                        return new_path

        # SDK头文件路径修复（per-package）
        if "per-package" in parts and "sdk" in parts:
            idx = parts.index("sdk")
            new_path = SOURCE_DIR / "sdk" / "include" / Path(*parts[idx + 1:])
            if new_path.exists():
                print(f"📦 SDK头文件映射: {file_path} -> {new_path}")
                return new_path

    except ValueError:
        pass
    return None


def fix_arguments(args):
    fixed_args = []
    for arg in args:
        if isinstance(arg, str) and "sdk/include" in arg and "per-package" in arg:
            real_sdk_path = str(SOURCE_DIR / "sdk" / "include")
            print(f"📁 替换 -I 路径: {arg} -> -I{real_sdk_path}")
            fixed_args.append("-I" + real_sdk_path)
        else:
            fixed_args.append(arg)
    return fixed_args

def merge_and_fix():
    print("\n🔧 合并所有 compile_commands 并修复路径")
    all_entries = []
    for file in Path(".").glob("compile_commands_*.json"):
        with open(file) as f:
            all_entries += json.load(f)

    new_entries = []
    for entry in all_entries:
        orig_file = Path(entry["file"])
        new_file = map_to_source(orig_file) or orig_file
        entry["file"] = str(new_file)
        entry["directory"] = str(SOURCE_DIR)

        if "arguments" in entry:
            entry["arguments"] = fix_arguments(entry["arguments"])

        new_entries.append(entry)

    FINAL_DB.parent.mkdir(parents=True, exist_ok=True)
    with open(FINAL_DB, "w") as f:
        json.dump(new_entries, f, indent=2)
    print(f"\n✅ 已生成: {FINAL_DB}")


if __name__ == "__main__":
    for module in BEAR_MODULES:
        build_with_bear(module)
    # build_sdk_with_bear()
    # generate_sdk_compile_commands()
    merge_and_fix()
