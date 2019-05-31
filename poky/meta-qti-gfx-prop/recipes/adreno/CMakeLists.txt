set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")

include_directories (${SYSROOT_INCDIR}/EGL)

add_definitions(
    -D_LINUX
)

add_subdirectory (eglWaylandSubdriver/waylandegl/build/cmake ${PROJECT_BINARY_DIR}/waylandegl)
add_subdirectory (eglWaylandSubdriver/build/cmake            ${PROJECT_BINARY_DIR}/eglSubDriverWayland)
