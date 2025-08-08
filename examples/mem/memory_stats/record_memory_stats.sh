#!/bin/bash

# Memory statistics recording script
# Records heap memory statistics during Keystone Enclave program execution

# Enter keystone examples directory
cd /usr/share/keystone/examples/ || { echo "Error: Cannot enter /usr/share/keystone/examples/ directory. Please check if the path exists or permissions." | tee -a "$LOG_FILE"; exit 1; }

# Configuration
HOST_PATH="" # host programs are in current directory, no path prefix needed
LOG_FILE="memory_stats_test.log" # Overall test log file
RESULTS_DIR="memory_stats_results" # Directory for CSV result files

# Define host program test modes
MODES=("malloc-large.ke gcbench.ke malloc-test.ke miniz.ke genann.ke simple-neural-network.ke") # All available user-defined test modes
# Store modes to run, empty by default, if user doesn't specify, run all modes
declare -a MODES_TO_RUN=()

# Show help information
show_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  --mode <program>   Specify program to run (e.g.: --mode simple-neural-network). Can use this option multiple times for multiple programs."
    echo "  --help            Show this help information."
    echo ""
    echo "If --mode option is not specified, all defined programs will run."
    echo "Script will record the following memory statistics:"
    echo "  - Heap used memory (bytes)"
    echo "  - Available memory (bytes)"
    echo "  - Total memory (bytes)"
    echo "  - Memory utilization (%)"
    echo "  - Timestamp (cycles)"
}

# 解析命令行参数
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --mode)
            if [[ -z "$2" ]]; then
                echo "Error: --mode option requires a program name as parameter." | tee -a "$LOG_FILE"
                show_help
                exit 1
            fi
            MODES_TO_RUN+=("$2")
            shift # consume program name parameter
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1" | tee -a "$LOG_FILE"
            show_help
            exit 1
            ;;
    esac
    shift # consume current option
done

# If no modes specified, run all defined modes
if [ ${#MODES_TO_RUN[@]} -eq 0 ]; then
    MODES_TO_RUN=("${MODES[@]}")
fi

# 检查并加载 keystone_driver 模块
check_and_load_keystone_driver() {
    echo "检查 keystone_driver 模块状态..." | tee -a "$LOG_FILE"
    if ! lsmod | grep -q keystone_driver; then
        echo "keystone_driver 模块未加载。尝试加载..." | tee -a "$LOG_FILE"
        modprobe keystone-driver # 使用 modprobe
        if [ $? -eq 0 ]; then
            echo "keystone_driver 模块加载成功。" | tee -a "$LOG_FILE"
        else
            echo "错误: 无法加载 keystone_driver 模块。请确保你有足够的权限且模块文件存在。" | tee -a "$LOG_FILE"
            exit 1
        fi
    else
        echo "keystone_driver 模块已加载。" | tee -a "$LOG_FILE"
    fi
}

# 在测试开始前检查并加载 keystone_driver
check_and_load_keystone_driver


echo "Starting memory statistics recording test..." | tee -a "$LOG_FILE"
echo "Programs to run: ${MODES_TO_RUN[*]}" | tee -a "$LOG_FILE"
echo "CSV format results will be saved to $RESULTS_DIR/ directory" | tee -a "$LOG_FILE"

# 创建结果目录
mkdir -p "$RESULTS_DIR"

# Stream processing function for memory statistics
# Reads from stdin and writes CSV data in real-time
stream_process_memory_stats() {
    local csv_file="$1"
    local mode="$2"
    local entry_count=0
    
    # Extract all memory statistics data
    local heap_used=""
    local available_memory=""
    local total_memory=""
    local memory_utilization=""
    local timestamp=""
    local in_memory_block=false

    while IFS= read -r line; do
        # Check if we're starting a new memory statistics block
        if [[ "$line" =~ ===.*Memory.*Statistics.*=== ]]; then
            in_memory_block=true
            # Reset all variables for new block
            heap_used=""
            available_memory=""
            total_memory=""
            memory_utilization=""
            timestamp=""
        elif [[ "$line" =~ ======================== ]]; then
            # End of memory statistics block
            in_memory_block=false
            
            # Write complete record if we have all required fields
            if [[ -n "$heap_used" && -n "$available_memory" && -n "$total_memory" && -n "$memory_utilization" && -n "$timestamp" ]]; then
                echo "$mode,$heap_used,$available_memory,$total_memory,$memory_utilization,$timestamp" >> "$csv_file"
                entry_count=$((entry_count + 1))
                echo "    [INFO] Saved memory stats entry #$entry_count to CSV" >&2
            fi
            
            # Reset for next block
            heap_used=""
            available_memory=""
            total_memory=""
            memory_utilization=""
            timestamp=""
        elif [[ "$in_memory_block" == true ]]; then
            # Parse individual fields within a memory statistics block
            if [[ "$line" =~ Heap\ used:\ +([0-9]+)\ bytes ]]; then
                heap_used="${BASH_REMATCH[1]}"
            elif [[ "$line" =~ Available\ memory:\ +([0-9]+)\ bytes ]]; then
                available_memory="${BASH_REMATCH[1]}"
            elif [[ "$line" =~ Total\ memory:\ +([0-9]+)\ bytes ]]; then
                total_memory="${BASH_REMATCH[1]}"
            elif [[ "$line" =~ Memory\ utilization:\ +([0-9]+\.[0-9]+)% ]]; then
                memory_utilization="${BASH_REMATCH[1]}"
            elif [[ "$line" =~ Timestamp:\ +([0-9]+)\ cycles ]]; then
                timestamp="${BASH_REMATCH[1]}"
            fi
        fi
    done
    
    # Handle case where the last block might not have ending marker
    if [[ "$in_memory_block" == true && -n "$heap_used" && -n "$available_memory" && -n "$total_memory" && -n "$memory_utilization" && -n "$timestamp" ]]; then
        echo "$mode,$heap_used,$available_memory,$total_memory,$memory_utilization,$timestamp" >> "$csv_file"
        entry_count=$((entry_count + 1))
        echo "    [INFO] Saved final memory stats entry #$entry_count to CSV" >&2
    fi
    
    # Return entry count via a temp file (since we can't return from a pipeline)
    echo "$entry_count" > "/tmp/memory_stats_count_$$"
}

# Run memory statistics test
run_memory_stats_test() {
    echo "Starting memory statistics data collection..." | tee -a "$LOG_FILE"
    
    for MODE in "${MODES_TO_RUN[@]}"; do
        echo "Testing program: $MODE" | tee -a "$LOG_FILE"
        
        # Create separate CSV file for each mode
        local mode_name=$(echo "$MODE" | sed 's/.ke$//')  # Remove .ke extension
        local csv_file="$RESULTS_DIR/${mode_name}_memory_stats_results.csv"
        echo "Results will be saved to $csv_file" | tee -a "$LOG_FILE"
        
        # Write CSV header
        echo "Program,Heap_Used_Bytes,Available_Memory_Bytes,Total_Memory_Bytes,Memory_Utilization_Percent,Timestamp_Cycles" > "$csv_file"
        
        echo "  Running..." | tee -a "$LOG_FILE"
        
        # Use tee to simultaneously display output and process it for CSV
        # The output goes to terminal AND gets processed for memory stats
        ./"$MODE" 2>&1 | tee -a "$LOG_FILE" | stream_process_memory_stats "$csv_file" "$MODE"
        
        # Get the entry count from temp file
        local entry_count=0
        if [[ -f "/tmp/memory_stats_count_$$" ]]; then
            entry_count=$(cat "/tmp/memory_stats_count_$$")
            rm -f "/tmp/memory_stats_count_$$"
        fi
        
        echo "    Found $entry_count memory statistics entries" | tee -a "$LOG_FILE"
        echo "" | tee -a "$LOG_FILE"
    done
}

# Execute memory statistics test
run_memory_stats_test

echo "Memory statistics recording test completed. Detailed log in $LOG_FILE, individual CSV results saved in $RESULTS_DIR/ directory."