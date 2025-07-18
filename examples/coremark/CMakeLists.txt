set(eapp_bin coremark)
set(eapp_src eapp/core_main.c eapp/core_state.c eapp/core_util.c eapp/core_matrix.c eapp/core_list_join.c eapp/core_portme.c)
set(host_bin coremark-runner)
set(host_src host/host.cpp)
set(package_name "coremark.ke")
set(package_script "./coremark-runner coremark eyrie-rt loader.bin")
set(eyrie_plugins "io_syscall linux_syscall env_setup")

# eapp

add_executable(${eapp_bin} ${eapp_src})
target_link_libraries(${eapp_bin} "-static")

set(FLAGS_STR " ")

add_compile_definitions(
  FLAGS_STR=\"${FLAGS_STR}\"
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
