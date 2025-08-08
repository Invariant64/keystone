set(eapp_bin malloc-large)
set(eapp_src eapp/malloc-large.c)
set(host_bin malloc-large-runner)
set(host_src host/host.cpp)
set(package_name "malloc-large.ke")
set(package_script "./malloc-large-runner malloc-large eyrie-rt loader.bin")
# set(eyrie_plugins "io_syscall linux_syscall env_setup PAGING DEBUG INTERNAL_STRACE")
set(eyrie_plugins "io_syscall linux_syscall env_setup ")

# eapp

add_executable(${eapp_bin} ${eapp_src})
target_link_libraries(${eapp_bin} "-static")

target_compile_options(${eapp_bin} PRIVATE -DKEYSTONE_ENABLE_TRACKING) # Enable tracking for memory stats

target_include_directories(${eapp_bin}
  PUBLIC ${KEYSTONE_SDK_DIR}/include/memory_stats
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
