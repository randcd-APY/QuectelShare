cmake_minimum_required(VERSION 2.8)

project(raw2bmp)

# it can be supported on OpenCV 3.x.x
# but this sample code is only verified on OpenCV 3.1.0
message(STATUS "Set your OpenCV lib path:")
set(OpenCV_DIR "/Downloads/opencv/build")

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)


# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")



if (${OpenCV_VERSION} LESS "3.0.0")
else (${OpenCV_VERSION} LESS "3.0.0")
add_definitions(-DOPENCV3)
endif (${OpenCV_VERSION} LESS "3.0.0")

# Add OpenCV headers location to your include paths
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
