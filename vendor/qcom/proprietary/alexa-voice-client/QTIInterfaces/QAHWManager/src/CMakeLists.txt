cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

add_library(QAHWManager SHARED
    QAHWManager.cpp)
target_include_directories(QAHWManager PUBLIC
    "${QAHWManager_SOURCE_DIR}/include"
    "${AVSCommon_INCLUDE_DIRS}"
    "${AIP_SOURCE_DIR}/include"
    "${QSTHWManager_SOURCE_DIR}/include"
    "${SYSROOT_DIR}/usr/include"
    "${SYSROOT_DIR}/usr/include/gstreamer-1.0"
    "${SYSROOT_DIR}/usr/include/glib-2.0"
    "${SYSROOT_DIR}/usr/lib/glib-2.0/include"
    )

target_link_libraries(QAHWManager AVSCommon AIP qgstaudiorecorder QSTHWManager DefaultClient)
INSTALL(TARGETS QAHWManager DESTINATION lib)
