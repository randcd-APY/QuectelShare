# ---------------------------------------------------------------------------
#                    Make the shared library (libchromatix_hi259_default_video)
# ---------------------------------------------------------------------------
#libchromatix_hi259_default_video

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/sensors
MM2_PATH = $(top_srcdir)/mm-camera2

lib_LTLIBRARIES = libchromatix_hi259_default_video.la

libchromatix_hi259_default_video_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libchromatix_hi259_default_video_la_CFLAGS += -fPIC

libchromatix_hi259_default_video_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301
libchromatix_hi259_default_video_la_CFLAGS += -I $(CURR_PATH)/module
libchromatix_hi259_default_video_la_CFLAGS += -I $(MM2_PATH)/../common
libchromatix_hi259_default_video_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301/chromatix_hi259/video

#TODO:
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

libchromatix_hi259_default_video_la_SOURCES = chromatix_hi259_video.c

#LOCAL_SHARED_LIBRARIES := libcutils
libchromatix_hi259_default_video_la_LDFLAGS = -shared
libchromatix_hi259_default_video_la_LIBADD = -ldl -llog
