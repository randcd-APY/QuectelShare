# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_preview)
# ---------------------------------------------------------------------------
#libchromatix_ov7251_preview

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/sensors
MM2_PATH = $(top_srcdir)/mm-camera2

lib_LTLIBRARIES = libchromatix_ov7251_preview.la

libchromatix_ov7251_preview_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
#libchromatix_ov7251_preview_la_CFLAGS += -fPIC

libchromatix_ov7251_preview_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301
libchromatix_ov7251_preview_la_CFLAGS += -I $(CURR_PATH)/module
libchromatix_ov7251_preview_la_CFLAGS += -I $(MM2_PATH)/../common
libchromatix_ov7251_preview_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301/chromatix_imx241/preview

#TODO:
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

libchromatix_ov7251_preview_la_SOURCES = chromatix_ov7251_preview.c

#LOCAL_SHARED_LIBRARIES := libcutils
libchromatix_ov7251_preview_la_LDFLAGS = -shared
libchromatix_ov7251_preview_la_LIBADD = -ldl -lcutils -llog
