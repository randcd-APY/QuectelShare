cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(Application LANGUAGES CXX)

#find_package(PkgConfig)

include(../build/BuildDefaults.cmake)

add_subdirectory("app")

