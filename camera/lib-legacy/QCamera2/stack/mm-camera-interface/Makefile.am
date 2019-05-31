ACLOCAL_AMFLAGS = -I m4

curr_path = $(top_srcdir)/QCamera2/stack/mm-camera-interface

AM_CFLAGS = -Wall -Wextra

AM_CPPFLAGS = \
    -I$(curr_path)/inc \
    -I$(curr_path)/../common \
    -I$(WORKSPACE)/hardware/libhardware/include/hardware \
    -I$(WORKSPACE)/system/media/camera/include

AM_CPPFLAGS += -DCAMERA_ION_HEAP_ID=ION_IOMMU_HEAP_ID
AM_CPPFLAGS += -DCAMERA_GRALLOC_HEAP_ID=GRALLOC_USAGE_PRIVATE_MM_HEAP
AM_CPPFLAGS += -DCAMERA_GRALLOC_FALLBACK_HEAP_ID=GRALLOC_USAGE_PRIVATE_IOMMU_HEAP
AM_CPPFLAGS += -DCAMERA_ION_FALLBACK_HEAP_ID=ION_IOMMU_HEAP_ID
AM_CPPFLAGS += -DCAMERA_GRALLOC_CACHING_ID=0
AM_CPPFLAGS += -DNUM_RECORDING_BUFFERS=9


AM_CPPFLAGS += -DDAEMON_PRESENT

# System header file path prefix
AM_CPPFLAGS += -DSYSTEM_HEADER_PREFIX=sys

#if TARGET_USES_ION
AM_CPPFLAGS += -DUSE_ION
#endif

#if VENUS_SUPPORTED
AM_CPPFLAGS += -DVENUS_PRESENT
#endif

#if UBWC_PRESENT
AM_CPPFLAGS += -DUBWC_PRESENT
#endif

	ifeq ($(KERNEL_VER), 4.9%)
#AM_CPPFLAGS += -DUSE_4_9_DEFS
	endif

AM_CPPFLAGS += -D_ANDROID_ -DQCAMERA_REDEFINE_LOG

AM_CPPFLAGS += -D__unused="__attribute__((__unused__))"
AM_CPPFLAGS += -include stdint.h
AM_CPPFLAGS += -include sys/ioctl.h

c_sources = \
        src/mm_camera_interface.c \
        src/mm_camera.c \
        src/mm_camera_channel.c \
        src/mm_camera_stream.c \
        src/mm_camera_thread.c \
        src/mm_camera_sock.c \
        src/cam_intf.c

h_sources = $(curr_path)/../common/cam_intf.h \
            $(curr_path)/../common/cam_types.h

pkgincludedir = $(includedir)/mm-camera-interface
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

lib_LTLIBRARIES = libmmcamera_interface.la
libmmcamera_interface_la_SOURCES = $(c_sources)
libmmcamera_interface_la_LIBADD = $(GLIB_LIBS) -ldl -lcutils -llog
libmmcamera_interface_la_CFLAGS = $(AM_CFLAGS)
libmmcamera_interface_la_CPPFLAGS = $(AM_CPPFLAGS)
libmmcamera_interface_la_LDFLAGS = -shared
