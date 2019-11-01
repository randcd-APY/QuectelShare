project(OTADEMO C)
cmake_minimum_required(VERSION 2.6.3)
#set ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_admin )

# package version
find_package (PkgConfig REQUIRED)

add_subdirectory(util)

enable_testing()

add_test(test_run
    util/ota-test
)
