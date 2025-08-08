#!/bin/bash

# 请确保 'bc' 工具已安装在你的系统中，否则脚本中的数学计算会失败。

# 进入 keystone examples 目录
cd /usr/share/keystone/examples/ || { echo "错误: 无法进入 /usr/share/keystone/examples/ 目录。请检查路径是否存在或权限。"; exit 1; }

# 配置
STRESS_PATH="stress-ng"
HOST_PATH="" # host 程序在当前目录，不再需要指定路径前缀
# N_VALUES=(0 1 2 4 8 16 32 64 128) # worker 数量的 N 值 (适用于 cpu, vm, io)，0 代表基线，按指数增长
declare -a N_VALUES=(0 1 2 4 8 16 32 64 128) # 可通过命令行参数覆盖

RESULTS_DIR="stress_results" # CSV 结果文件存放的目录

# 标记要运行的测试类型
RUN_CPU=false
RUN_VM=false
RUN_IO=false
RUN_VM_HANG=false # VM-hang 测试
RUN_HDD=false # 新增：HDD (磁盘) 压力测试
RUN_ALL=true # 默认情况下，如果没有指定选项，则运行所有测试

# 定义 host 程序的测试模式
MODES=("aes.ke" "bigint.ke" "dhrystone.ke" "miniz.ke" "norx.ke" "primes.ke" "qsort.ke" "sha512.ke" "iozone.ke") # 所有可用的用户定义测试模式
# 存储将要运行的模式，默认情况下为空，如果用户没有指定，则运行所有模式
declare -a MODES_TO_RUN=()

# 显示帮助信息
show_help() {
    echo "用法: $0 [选项]"
    echo "选项:"
    echo "  --cpu       运行 CPU 压力测试。"
    echo "  --vm        运行 VM (内存) 压力测试。"
    echo "  --io        运行 I/O 压力测试。"
    echo "  --vm-hang   运行 VM-hang (内存一直开辟不释放) 压力测试。"
    echo "  --hdd       运行 HDD (磁盘) 压力测试。"
    echo "  --mode <程序名>   指定要运行的程序 (例如: --mode dhrystone.ke)。可以多次使用此选项来指定多个程序。"
    echo "  --n-list <N1,N2,...> 指定 stress worker 数量的 N 值列表 (例如: --n-list \"0,1,2,4\")。0 代表基线。"
    echo "  --help      显示此帮助信息。"
    echo ""
    echo "如果未指定任何压力选项，将运行所有测试 (CPU、VM、I/O、VM-hang、HDD)。"
    echo "如果未指定 --mode 选项，将运行所有定义的程序。"
    echo "N=0 表示不施加 stress 压力，作为基线对比。"
}

# 解析命令行参数
# 使用 while 循环和 shift 来正确处理带参数的选项
while [[ "$#" -gt 0 ]]; do
    case "$1" in
        --cpu)
            RUN_CPU=true
            RUN_ALL=false
            ;;
        --vm)
            RUN_VM=true
            RUN_ALL=false
            ;;
        --io)
            RUN_IO=true
            RUN_ALL=false
            ;;
        --vm-hang)
            RUN_VM_HANG=true
            RUN_ALL=false
            ;;
        --hdd)
            RUN_HDD=true
            RUN_ALL=false
            ;;
        --mode)
            if [[ -z "$2" ]]; then
                echo "错误: --mode 选项需要一个程序名作为参数。"
                show_help
                exit 1
            fi
            MODES_TO_RUN+=("$2")
            shift # 消耗程序名参数
            ;;
        --n-list)
            if [[ -z "$2" ]]; then
                echo "错误: --n-list 选项需要一个逗号分隔的 N 值列表作为参数。"
                show_help
                exit 1
            fi
            IFS=',' read -r -a N_VALUES <<< "$2" # 将逗号分隔的字符串读取到 N_VALUES 数组中
            shift # 消耗 N 值列表参数
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            echo "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
    shift # 消耗当前选项
done

# 如果没有指定要运行的模式，则运行所有默认模式
if [ ${#MODES_TO_RUN[@]} -eq 0 ]; then
    MODES_TO_RUN=("${MODES[@]}")
fi

# 如果 RUN_ALL 仍然为 true，则将所有测试标记设置为 true
if $RUN_ALL; then
    RUN_CPU=true
    RUN_VM=true
    RUN_IO=true
    RUN_VM_HANG=true
    RUN_HDD=true # 新增：如果运行所有测试，也包含 HDD 测试
fi

# 检查并加载 keystone_driver 模块
check_and_load_keystone_driver() {
    echo "检查 keystone_driver 模块状态..."
    if ! lsmod | grep -q keystone_driver; then
        echo "keystone_driver 模块未加载。尝试加载..."
        modprobe keystone-driver # 使用 modprobe
        if [ $? -eq 0 ]; then
            echo "keystone_driver 模块加载成功。"
        else
            echo "错误: 无法加载 keystone_driver 模块。请确保你有足够的权限且模块文件存在。"
            exit 1
        fi
    else
        echo "keystone_driver 模块已加载。"
    fi
}

# 在测试开始前检查并加载 keystone_driver
check_and_load_keystone_driver

# 创建结果目录（如果不存在）
mkdir -p "$RESULTS_DIR"
echo "测试开始，CSV 格式结果将保存到 $RESULTS_DIR/ 目录下"
echo ""

# 运行单一类型压力测试的函数 (cpu, vm, io, vm_hang)
# 参数: $1 = 压力类型 (例如: "cpu", "vm", "io", "vm_hang")
#       $2 = stress 命令的主参数 (例如: "--cpu", "--vm", "--io")
#       $3... = stress 命令的额外参数 (例如: "--vm-hang", "0")
run_stress_test() {
    local stress_type="$1"
    local stress_main_arg="$2"
    shift 2 # 移除前两个参数，剩下的是额外参数
    local stress_extra_args=("$@") # 将剩余参数存储到数组中

    local csv_file_prefix="$RESULTS_DIR/${stress_type}"

    echo "========================================"
    echo "开始运行 ${stress_type} 压力测试"
    echo "========================================"
    echo ""

    # 循环遍历每个 host 程序模式
    for MODE in "${MODES_TO_RUN[@]}"; do
        local csv_file="${csv_file_prefix}_${MODE}_results.csv"
        echo "----------------------------------------"
        echo "正在测试程序: $MODE (压力类型: ${stress_type})"
        echo "结果将保存到 $csv_file"
        echo "----------------------------------------"

        # 立即写入 CSV 表头：Program,N=0 (Baseline),N=1,N=2,...
        local header="Program"
        for N_val in "${N_VALUES[@]}"; do
            if [ "$N_val" -eq 0 ]; then
                header+=",N=0 (Baseline)"
            else
                header+=",N=$N_val"
            fi
        done
        echo "$header" > "$csv_file"

        # 准备当前程序的数据行
        local row="$MODE"

        # 循环遍历当前压力类型的 N 值
        for N in "${N_VALUES[@]}"; do
            echo "  --------------------------------------"
            if [ "$N" -eq 0 ]; then
                echo "  运行基线测试 (N=0)，不启动 stress 进程。"
                STRESS_PID="" # 基线测试没有 stress PID
            else
                echo "  启动 stress ${stress_main_arg} $N ${stress_extra_args[*]}"
                # 在后台启动 stress，并获取其 PID
                "$STRESS_PATH" "$stress_main_arg" "$N" "${stress_extra_args[@]}" &
                STRESS_PID=$!
                echo "  Stress 进程已启动，PID: $STRESS_PID"
                # 等待几秒，让负载稳定
                echo "  等待 stress 负载稳定 (5秒)..."
                sleep 5
            fi
            echo "  --------------------------------------"

            local total_real_time=0
            local valid_runs_count=0
            
            echo "    运行 host $MODE..."
            # 运行程序，同时显示到终端和捕获输出
            ENCLAVE_OUTPUT=$(./"$MODE" 2>&1)
            echo "$ENCLAVE_OUTPUT"  # 显示到终端
            
            REAL_TIME_NS_STR=$(echo "$ENCLAVE_OUTPUT" | grep 'Enclave run time:' | awk '{print $4}') # 提取纳秒值
            
            local REAL_TIME=0
            if [[ -n "$REAL_TIME_NS_STR" && "$REAL_TIME_NS_STR" =~ ^[0-9]+$ ]]; then
                REAL_TIME=$(echo "scale=3; $REAL_TIME_NS_STR / 1000000000" | bc) # 转换为秒
            fi

            if (( $(echo "$REAL_TIME > 0" | bc -l) )); then # 只有当秒数大于0时才计入平均值
                total_real_time=$(echo "$total_real_time + $REAL_TIME" | bc)
                valid_runs_count=$((valid_runs_count + 1))
                echo "    实际运行时间: $REAL_TIME 秒"
            else
                echo "    实际运行时间: $REAL_TIME 秒 (忽略，因秒数为0或解析失败)"
            fi

            local average_real_time
            if [ "$valid_runs_count" -gt 0 ]; then
                average_real_time=$(echo "scale=3; $total_real_time / $valid_runs_count" | bc)
            else
                average_real_time="N/A" # 所有运行都为0秒，记录N/A
            fi
            
            # 立即追加结果到数据行
            row+=",$average_real_time"
            echo "  程序 $MODE 在 N=$N 时的平均实际运行时间: $average_real_time 秒"

            # 停止 stress 进程 (仅当 stress 实际启动时才停止)
            if [ -n "$STRESS_PID" ]; then # 检查 STRESS_PID 是否非空
                echo "  停止 stress 进程 (PID: $STRESS_PID)..."
                kill "$STRESS_PID"
                wait "$STRESS_PID" 2>/dev/null
                echo "  Stress 进程已停止。"
            else
                echo "  N=0 基线测试完成，未启动 stress 进程。"
            fi
            echo ""

        done # N_VALUES 循环结束

        # 立即写入当前程序的完整数据行到CSV文件
        echo "$row" >> "$csv_file"
        echo "CSV 结果已写入 $csv_file 文件中。"
        echo ""

    done # MODES 循环结束

    echo "${stress_type} 测试已完成。"
    echo ""
}

# 根据标记执行测试
if $RUN_CPU; then
    run_stress_test "cpu" "--cpu" "--cpu-method" "all"
fi

if $RUN_VM; then
    run_stress_test "vm" "--vm"
fi

if $RUN_IO; then
    run_stress_test "io" "--iomix"
fi

if $RUN_VM_HANG; then
    run_stress_test "vm_hang" "--vm" "--vm-hang" "0" "--vm-bytes" "64M"
fi

# 新增：运行 HDD 压力测试
if $RUN_HDD; then
    run_stress_test "hdd" "--hdd" "--hdd-bytes" "16M"
fi

echo "所有选定的测试已完成。CSV 结果在 $RESULTS_DIR/ 目录中。" 