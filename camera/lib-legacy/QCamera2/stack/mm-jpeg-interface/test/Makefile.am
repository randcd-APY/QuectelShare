ACLOCAL_AMFLAGS = -I m4

curr_path = $(top_srcdir)/QCamera2/stack/mm-jpeg-interface/test

bin_PROGRAMS = mm-jpeg-interface-test

AM_CFLAGS = -Wall -Wextra -Werror -Wno-unused-parameter

AM_CPPFLAGS = -DCAMERA_ION_HEAP_ID=ION_IOMMU_HEAP_ID
AM_CPPFLAGS += -D_ANDROID_
AM_CPPFLAGS += \
        -I ./ \
        -I $(WORKSPACE)/frameworks/native/include/media/openmax \
        -I $(curr_path)/../inc \
        -I $(curr_path)/../../common \
        -I $(curr_path)/../../mm-camera-interface/inc \
        -I $(curr_path)/../../.. \
        -I $(curr_path)/../../../../mm-image-codec/qexif \
        -I $(curr_path)/../../../../mm-image-codec/qomx_core

if TARGET_USES_ION
AM_CPPFLAGS += -DUSE_ION
endif

if TARGET_SUPPORTS_DUAL_JPEG
AM_CPPFLAGS += -DMM_JPEG_CONCURRENT_SESSIONS_COUNT=2
else
AM_CPPFLAGS += -DMM_JPEG_CONCURRENT_SESSIONS_COUNT=1
endif

if JPEG_USE_PIPELINE
AM_CPPFLAGS += -DMM_JPEG_USE_PIPELINE
endif

AM_CPPFLAGS += -DSYSTEM_HEADER_PREFIX=sys

mm_jpeg_interface_test_SOURCES = mm_jpeg_test.c
mm_jpeg_interface_test_CFLAGS = $(AM_CFLAGS)
mm_jpeg_interface_test_CPPFLAGS = $(AM_CPPFLAGS)
mm_jpeg_interface_test_LDADD = $(GLIB_LIBS) -lcutils -ldl -llog
mm_jpeg_interface_test_LDADD += ../libmmjpeg_interface.la

bin_PROGRAMS += mm-jpegdec-interface-test

LOCAL_CPPFLAGS = -DCAMERA_ION_HEAP_ID=ION_IOMMU_HEAP_ID
LOCAL_CPPFLAGS += -D_ANDROID_
LOCAL_CPPFLAGS += \
        -I ./ \
        -I $(WORKSPACE)/frameworks/native/include/media/openmax \
        -I $(curr_path)/../inc \
        -I $(curr_path)/../../common \
        -I $(curr_path)/../../mm-camera-interface/inc \
        -I $(curr_path)/../../.. \
        -I $(curr_path)/../../../../mm-image-codec/qexif \
        -I $(curr_path)/../../../../mm-image-codec/qomx_core

if TARGET_USES_ION
LOCAL_CPPFLAGS += -DUSE_ION
endif

# System header file path prefix
LOCAL_CPPFLAGS += -DSYSTEM_HEADER_PREFIX=sys

mm_jpegdec_interface_test_SOURCES = mm_jpegdec_test.c
mm_jpegdec_interface_test_CFLAGS = $(AM_CFLAGS)
mm_jpegdec_interface_test_CPPFLAGS = $(LOCAL_CPPFLAGS)
mm_jpegdec_interface_test_LDADD = $(GLIB_LIBS) -lcutils -ldl -llog
mm_jpegdec_interface_test_LDADD += ../libmmjpeg_interface.la
