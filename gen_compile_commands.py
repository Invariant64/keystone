#!/usr/bin/python3

import json
import os
import re

# 模块替换规则，键是模块名，值是源码目录路径
module_map = {
    "keystone-sm": "/home/noah/tee-exp/keystone/sm",
    "keystone-runtime": "/home/noah/tee-exp/keystone/runtime",
    "keystone-driver": "/home/noah/tee-exp/keystone/linux-keystone-driver",
    "keystone-examples": "/home/noah/tee-exp/keystone/examples",
    "host-keystone-sdk": "/home/noah/tee-exp/keystone/sdk",
    "opensbi-custom": "/home/noah/tee-exp/keystone/opensbi-1.1",
}

# 构建目录前缀（路径前半段）
build_root = "/home/noah/tee-exp/keystone/build-generic64/buildroot.build/build"

# 匹配形如 keystone-sm-xxxxx 的路径
pattern = re.compile(rf"{re.escape(build_root)}/(?P<modname>[^/]+?)-[0-9a-f]+(/|$)")
# opensbi 没有哈希目录，直接匹配
pattern_opensbi = re.compile(rf"{re.escape(build_root)}/opensbi-custom(/|$)")

def replace_path(path):
    match = pattern_opensbi.search(path)
    if match:
        # 如果是 opensbi-custom 的路径，直接替换为对应的源码路径
        return os.path.join(module_map["opensbi-custom"], path[match.end():].lstrip("/"))

    match = pattern.search(path)
    if not match:
        return path

    mod_key = match.group("modname")
    new_base = module_map.get(mod_key)
    if not new_base:
        return path

    # 将匹配到的 build 前缀和模块名部分替换为新的源码路径
    suffix = path[match.end():]  # 剩下的路径部分（哈希目录之后）
    return os.path.join(new_base, suffix.lstrip("/"))

def process_entry(entry):
    for key in ["file", "directory", "output"]:
        if key in entry:
            entry[key] = replace_path(entry[key])

    if "arguments" in entry:
        entry["arguments"] = [replace_path(arg) for arg in entry["arguments"]]
    if "command" in entry:
        entry["command"] = replace_path(entry["command"])

    return entry

def process_compile_commands(input_path, output_path=None):
    with open(input_path, 'r') as f:
        data = json.load(f)

    new_data = [process_entry(entry) for entry in data]

    out_path = output_path if output_path else input_path
    with open(out_path, 'w') as f:
        json.dump(new_data, f, indent=2)

    print(f"Updated compile_commands.json saved to: {out_path}")

# 使用示例
if __name__ == "__main__":
    process_compile_commands("compile_commands.json")
