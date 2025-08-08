# RV8-Bench Suite
# Collection of RISC-V benchmarks from the rv8 project

# RV8-Bench targets
add_custom_target("rv8-bench")

# Crypto/Hash algorithms
add_subdirectory(aes)
add_subdirectory(sha512)
add_subdirectory(norx)

# System benchmarks  
add_subdirectory(dhrystone)
add_subdirectory(primes)

# Data processing
add_subdirectory(bigint)
add_subdirectory(miniz)
add_subdirectory(qsort)
