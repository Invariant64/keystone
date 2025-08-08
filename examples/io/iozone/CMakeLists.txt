set(eapp_bin iozone)
set(eapp_src eapp/iozone.c eapp/libasync.c eapp/libbif.c)
set(host_bin iozone-runner)
set(host_src host/host.cpp)
set(package_name "iozone.ke")
set(package_script "./iozone-runner iozone eyrie-rt loader.bin")
# set(eyrie_plugins "io_syscall linux_syscall env_setup PAGING INTERNAL_STRACE DEBUG")
set(eyrie_plugins "io_syscall linux_syscall env_setup DEBUG INTERNAL_STRACE")

# eapp

add_executable(${eapp_bin} ${eapp_src})
target_compile_definitions(${eapp_bin} PUBLIC
    NAME=\"linux\"
    _LARGEFILE64_SOURCE
    _FILE_OFFSET_BITS=64
    unix
    HAVE_ANSIC_C
    ASYNC_IO
    HAVE_PREAD
    SHARED_MEM
	  linux
    DONT_HAVE_O_DIRECT
)
target_compile_options(${eapp_bin} PUBLIC
    -Wno-implicit-function-declaration
    -Wno-incompatible-pointer-types
    -Wno-int-to-pointer-cast
)
target_link_libraries(${eapp_bin} "-static")

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