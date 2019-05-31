cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Set project information
project(QTISDK VERSION 1.0.2 LANGUAGES CXX)
set(PROJECT_BRIEF "A cross-platform, modular SDK from QTI")
set(INCLUDE_PATH "${SYSROOT_DIR}/usr/include")

include(build/BuildDefaults.cmake)

# Alexa Client SDK targets.
add_subdirectory("Application")
add_subdirectory("QTIInterfaces")

