# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx241_common)
# ---------------------------------------------------------------------------
#libchromatix_imx241_common

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/sensors
MM2_PATH = $(top_srcdir)/mm-camera2

lib_LTLIBRARIES = libchromatix_imx241_common.la

libchromatix_imx241_common_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
#libchromatix_imx241_common_la_CFLAGS += -fPIC

libchromatix_imx241_common_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301/
libchromatix_imx241_common_la_CFLAGS += -I $(CURR_PATH)/module
libchromatix_imx241_common_la_CFLAGS += -I $(MM2_PATH)/../common
libchromatix_imx241_common_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301/chromatix_imx241/common

#TODO:
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

libchromatix_imx241_common_la_SOURCES = chromatix_imx241_common.c

#LOCAL_SHARED_LIBRARIES := libcutils
libchromatix_imx241_common_la_LDFLAGS = -shared
libchromatix_imx241_common_la_LIBADD = -ldl -lcutils
