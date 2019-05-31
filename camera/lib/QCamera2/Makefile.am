ACLOCAL_AMFLAGS = -I m4
curr_path = $(top_srcdir)/QCamera2

h_sources = $(curr_path)/QCameraFormat.h
pkgincludedir = $(includedir)/qcom/camera
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

AM_CPPFLAGS = \
        -I$(curr_path) \
        -I$(curr_path)/../mm-image-codec/qexif \
        -I$(curr_path)/../mm-image-codec/qomx_core \
        -I$(curr_path)/include \
        -I$(curr_path)/stack/common \
        -I$(curr_path)/stack/mm-camera-interface/inc \
        -I$(curr_path)/util \
        -I$(curr_path)/HAL3 \
        -I$(curr_path)/HAL \
        -I$(WORKSPACE)/hardware/libhardware/include/hardware \
        -I$(WORKSPACE)/hardware/qcom/media/libstagefrighthw \
        -I$(WORKSPACE)/hardware/qcom/media/mm-core/inc \
        -I$(WORKSPACE)/system/core/include/cutils \
        -I$(WORKSPACE)/system/core/include \
        -I$(WORKSPACE)/system/core/include/system \
        -I$(WORKSPACE)/system/media/camera/include/system \
        -I$(WORKSPACE)/display/display-hal/libqservice \
        -I$(WORKSPACE)/display/display-hal/libqdutils \
        -I$(WORKSPACE)/disregard/frameworks/native/include \
        -I$(COMMON_INCDIR)/qcom/display


cpp_source = \
       util/QCameraBufferMaps.cpp \
       util/QCameraCmdThread.cpp \
       util/QCameraFlash.cpp \
       util/QCameraPerf.cpp \
       util/QCameraQueue.cpp \
       util/QCameraCommon.cpp \
       util/QCameraTrace.cpp \
       util/camscope_packet_type.cpp \
       QCamera2Factory.cpp \
       QCamera2Hal.cpp

hal3_source = \
       HAL3/QCamera3Channel.cpp \
       HAL3/QCamera3CropRegionMapper.cpp \
       HAL3/QCamera3HWI.cpp \
       HAL3/QCamera3Mem.cpp \
       HAL3/QCamera3PostProc.cpp \
       HAL3/QCamera3Stream.cpp \
       HAL3/QCamera3StreamMem.cpp \
       HAL3/QCamera3VendorTags.cpp

if DRONE_TARGET
AM_CPPFLAGS += -D_DRONE_
endif

if CAMERA_DEBUG_DATA
AM_CPPFLAGS += -DCAMERA_DEBUG_DATA
endif

AM_CPPFLAGS += -DQCAMERA_HAL3_SUPPORT

# System header file path prefix
AM_CPPFLAGS += -DSYSTEM_HEADER_PREFIX=sys

AM_CPPFLAGS += -DHAS_MULTIMEDIA_HINTS -D_ANDROID

AM_CPPFLAGS += -DUSE_HAL_3_3

if TARGET_USES_MEDIA_EXTENSIONS
AM_CPPFLAGS += -DUSE_MEDIA_EXTENSIONS
endif

AM_CPPFLAGS += -DDEFAULT_DENOISE_MODE_ON -DHAL3 -DQCAMERA_REDEFINE_LOG

if TARGET_TS_MAKEUP
AM_CPPFLAGS += -DTARGET_TS_MAKEUP
AM_CPPFLAGS += -I$(curr_path)/HAL/tsMakeuplib/include
endif

if VENUS_SUPPORTED
AM_CPPFLAGS += -DVENUS_PRESENT
endif

if UBWC_PRESENT
AM_CPPFLAGS += -DUBWC_PRESENT
endif

if MSM8996
AM_CPPFLAGS += -DTARGET_MSM8996
endif

if MSM8998
AM_CPPFLAGS += -DTARGET_MSM8998
AM_CPPFLAGS += -DQCAMERA_DUMP
endif

AM_CPPFLAGS += -DUSE_CAMERA_METABUFFER_UTILS

AM_CPPFLAGS += -D__unused="__attribute__((__unused__))"

AM_CPPFLAGS += -include stdint.h
AM_CPPFLAGS += -include glib.h
AM_CPPFLAGS += -include glibconfig.h
AM_CPPFLAGS += -include sys/ioctl.h
AM_CPPFLAGS += -include cutils/trace.h
AM_CPPFLAGS += -include sys/stat.h

AM_CXXFLAGS = -Wall -Wextra -Werror

lib_LTLIBRARIES = libcamera_default.la
libcamera_default_la_SOURCES = $(cpp_source) $(hal3_source)
libcamera_default_la_LIBADD  = $(GLIB_LIBS) -lcamera_client -llog -ldl -lsync -lui -lqdMetaData \
                               -lbinder -lbase -lutils -lcamera_metadata -lqservice -lhardware -lrt

libcamera_default_la_LDFLAGS = -shared
libcamera_default_la_LIBADD += stack/mm-camera-interface/libmmcamera_interface.la
libcamera_default_la_LIBADD += stack/mm-jpeg-interface/libmmjpeg_interface.la

libcamera_default_la_CPPFLAGS = $(AM_CPPFLAGS)
libcamera_default_la_CXXFLAGS = $(AM_CXXFLAGS)
