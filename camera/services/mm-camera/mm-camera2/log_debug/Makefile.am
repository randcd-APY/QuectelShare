# ==============================================================================
# ---------------------------------------------------------------------------------
#           Make the libmmcamera_dbg.so
# ---------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4
LOCAL_PATH = $(top_srcdir)

# trace logging lib
LOCAL_CFLAGS = -D_ANDROID_
LOCAL_CFLAGS += -DMEMLEAK_FLAG
LOCAL_CFLAGS += -Werror -Wunused-parameter

#************* MCT headers ************#
LOCAL_C_INCLUDES  = -I$(MM_CAMERA_PATH)/mm-camera2/includes
LOCAL_C_INCLUDES += -I$(LOCAL_PATH)/../media-controller/mct/debug/memleak/
LOCAL_C_INCLUDES += -I$(LOCAL_PATH)/../media-controller/mct/debug/fdleak/

#************* HAL headers ************#
LOCAL_C_INCLUDES +=  -I$(CAMERAHAL_PATH)/QCamera2/stack/common

#************* Source ************#
LOCAL_SRC_FILES = $(LOCAL_PATH)/android/camera_dbg.c
LOCAL_SRC_FILES += $(LOCAL_PATH)/android/camscope_packet_type.c
LOCAL_SRC_FILES += $(LOCAL_PATH)/../media-controller/mct/debug/memleak/override_malloc.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../media-controller/mct/debug/fdleak/mct_debug_fdleak.cpp

AM_CPPFLAGS = -include signal.h
AM_CPPFLAGS += -include sys/time.h
AM_CPPFLAGS += -D__unused="__attribute__((__unused__))"

LOCAL_LDFLAGS = -shared
lib_LTLIBRARIES = libmmcamera_dbg.la
libmmcamera_dbg_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera_dbg_la_LDFLAGS = $(LOCAL_LDFLAGS)
libmmcamera_dbg_la_LIBADD = -ldl -lcutils
libmmcamera_dbg_la_CPPFLAGS = $(GLIB_CFLAGS) $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) $(AM_CPPFLAGS)
