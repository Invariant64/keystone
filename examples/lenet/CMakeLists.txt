set(eapp_bin lenet)
set(eapp_src eapp/lenet.c)
set(host_bin lenet-runner)
set(host_src host/host.cpp)
set(package_name "lenet.ke")
set(package_script "./lenet-runner lenet eyrie-rt loader.bin")
set(eyrie_plugins "io_syscall linux_syscall env_setup")


set(TH_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/TH")
set(TH_BUILD_DIR "${TH_ROOT}/build")
set(TH_LIB_DIR "${TH_BUILD_DIR}")
set(TH_INCLUDE_DIR "${TH_ROOT}")


# 定义 LAPACK 路径
set(LAPACK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/lapack-3.12.1")
set(LAPACK_BUILD_DIR "${LAPACK_ROOT}/build")
set(LAPACK_LIB_DIR "${LAPACK_BUILD_DIR}/lib")
set(LAPACK_INCLUDE_DIR "${LAPACK_ROOT}")

# 检查TH库是否存在
if(NOT EXISTS "${TH_LIB_DIR}/libTH.a")
    message(FATAL_ERROR "TH library not found at ${TH_LIB_DIR}/libTH.a")
endif()

if(NOT EXISTS "${TH_INCLUDE_DIR}")
    message(FATAL_ERROR "TH headers not found at ${TH_INCLUDE_DIR}")
endif()

# 添加头文件路径
include_directories(${TH_BUILD_DIR})
include_directories(${TH_INCLUDE_DIR})


# eapp

add_executable(${eapp_bin} ${eapp_src})
target_link_libraries(${eapp_bin} "-static")

# 链接库
target_link_libraries(${eapp_bin} 
    "-static"
    "${TH_LIB_DIR}/libTH.a"
    "-lm"
    "-fopenmp"
    "${LAPACK_LIB_DIR}/libblas.a"
    "${LAPACK_LIB_DIR}/liblapack.a"
)

# host

add_executable(${host_bin} ${host_src})
target_link_libraries(${host_bin} ${KEYSTONE_LIB_HOST} ${KEYSTONE_LIB_EDGE})

# add target for Eyrie runtime (see keystone.cmake)

set(eyrie_files_to_copy .options_log eyrie-rt loader.bin)
add_eyrie_runtime(${eapp_bin}-eyrie
  ${eyrie_plugins}
  ${eyrie_files_to_copy})

# add target for packaging (see keystone.cmake)

add_keystone_package(${eapp_bin}-package
  ${package_name}
  ${package_script}
  ${eyrie_files_to_copy} ${eapp_bin} ${host_bin})

add_dependencies(${eapp_bin}-package ${eapp_bin}-eyrie)

# add package to the top-level target
add_dependencies(examples ${eapp_bin}-package)
