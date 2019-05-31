CLOCAL_AMFLAGS = -I m4

curr_path = $(top_srcdir)/QCamera2/stack/mm-camera-test

AM_CFLAGS = -Wall -Wextra -Werror

AM_CPPFLAGS = \
        -I$(curr_path)/inc \
        -I$(WORKSPACE)/frameworks/native/include/media/openmax \
        -I$(curr_path)/../common \
        -I$(curr_path)/../mm-camera-interface/inc \
        -I$(curr_path)/../../../mm-image-codec/qexif \
        -I$(curr_path)/../../../mm-image-codec/qomx_core

AM_CPPFLAGS += -D_ANDROID_ -DUSE_ION -DCAMERA_ION_HEAP_ID=ION_IOMMU_HEAP_ID

#if MSM8909
AM_CPPFLAGS += -DCAMERA_GRALLOC_HEAP_ID=GRALLOC_USAGE_PRIVATE_MM_HEAP
AM_CPPFLAGS += -DCAMERA_GRALLOC_FALLBACK_HEAP_ID=GRALLOC_USAGE_PRIVATE_IOMMU_HEAP
AM_CPPFLAGS += -DCAMERA_ION_FALLBACK_HEAP_ID=ION_IOMMU_HEAP_ID
AM_CPPFLAGS += -DCAMERA_GRALLOC_CACHING_ID=0
AM_CPPFLAGS += -DNUM_RECORDING_BUFFERS=9
#endif

AM_CPPFLAGS += -D__unused="__attribute__((__unused__))"
AM_CPPFLAGS += -include sys/ioctl.h
AM_CPPFLAGS += -include stdint.h

c_sources = \
        src/mm_qcamera_main_menu.c \
        src/mm_qcamera_app.c \
        src/mm_qcamera_unit_test.c \
        src/mm_qcamera_video.c \
        src/mm_qcamera_preview.c \
        src/mm_qcamera_snapshot.c \
        src/mm_qcamera_rdi.c \
        src/mm_qcamera_reprocess.c\
        src/mm_qcamera_queue.c \
        src/mm_qcamera_socket.c \
        src/mm_qcamera_commands.c

bin_PROGRAMS = mm-qcamera-app
mm_qcamera_app_SOURCES = $(c_sources)
mm_qcamera_app_LDADD = $(GLIB_LIBS) -llog -lcutils -ldl
mm_qcamera_app_LDADD += ../mm-camera-interface/libmmcamera_interface.la
mm_qcamera_app_CFLAGS = $(AM_CFLAGS)
mm_qcamera_app_CPPFLAGS = $(AM_CPPFLAGS)

lib_LTLIBRARIES = libmm-qcamera.la
libmm_qcamera_la_SOURCES = $(c_sources)
libmm_qcamera_la_LIBADD = $(GLIB_LIBS) -llog -lcutils -ldl
libmm_qcamera_la_LIBADD += ../mm-camera-interface/libmmcamera_interface.la
libmm_qcamera_la_CFLAGS = $(AM_CFLAGS)
libmm_qcamera_la_CPPFLAGS = $(AM_CPPFLAGS)
