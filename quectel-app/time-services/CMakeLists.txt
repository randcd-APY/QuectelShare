project(time_daemon C)
cmake_minimum_required(VERSION 2.6.3)
#set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_admin )

# package version
find_package (PkgConfig REQUIRED)

#pkg_check_modules ( QL-MCM-API REQUIRED ql_mcm_api )

add_subdirectory(src)

enable_testing()

add_test(time_daemon
    src/time_daemon
)
