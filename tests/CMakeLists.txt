project(uffs_ebm_libubiio_tests)

link_directories(test_ubiio ${uffs_ebm_userspace_SOURCE_DIR}/libubiio)
add_executable(test_ubiio test.c)
target_link_libraries(test_ubiio ubiio)

