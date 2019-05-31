cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Set project information
project(QTIInterfaces VERSION 1.0.2 LANGUAGES CXX)
set(PROJECT_BRIEF "QTI Interfaces")

find_package(PkgConfig)

pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.8 gstreamer-app-1.0>=1.8)

if(NOT PKG_CONFIG_FOUND)
    message("GStreamer not found. Skip building the GStreamer based media player.")
    return()
endif()

include(../build/BuildDefaults.cmake)

# Alexa Client SDK targets.
add_subdirectory("AVSManager")
add_subdirectory("QAHWManager")
add_subdirectory("QSTHWManager")

