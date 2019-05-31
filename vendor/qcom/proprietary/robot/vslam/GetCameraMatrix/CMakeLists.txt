cmake_minimum_required(VERSION 2.8)

project(GetCameraMatrix)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Fisheye mode calibration is supported from OpenCV 3.x
# and this sample code is only verified on OpenCV 3.1.0

#set(OpenCV_DIR "/usr/share/opencv/opencv-3.1.0/build")

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if (${OpenCV_VERSION} LESS "3.0.0")
else (${OpenCV_VERSION} LESS "3.0.0")
add_definitions(-DOPENCV3)
endif (${OpenCV_VERSION} LESS "3.0.0")

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(inc)

set( SRC_LIST
     src/main.cpp
     src/ReadSaveCameraParas.cpp
     src/UndistortionEngine.cpp
    )

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
target_link_libraries( ${PROJECT_NAME} ${GLIB_LIBS} )
