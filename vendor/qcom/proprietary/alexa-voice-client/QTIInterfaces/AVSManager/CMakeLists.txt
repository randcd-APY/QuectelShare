option(GSTREAMER_MEDIA_PLAYER "Enable GStreamer based media player." OFF)

option(HWMSM8909 "Enable GStreamer based media player." OFF)
if(NOT GSTREAMER_MEDIA_PLAYER)
    message("Skip building the GStreamer based media player.")
    return()
endif()
if(HWMSM8909)
    add_definitions(-DHWMSM8909)
endif()

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(AVSManager LANGUAGES CXX)

add_definitions(-DGSTREAMER_MEDIA_PLAYER)
include(../../build/BuildDefaults.cmake)

add_subdirectory("src")
