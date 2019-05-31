#libmmcamera_ov5640.so

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/sensors
MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2

lib_LTLIBRARIES = libmmcamera_ov5640.la

libmmcamera_ov5640_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera_ov5640_la_CFLAGS += -fPIC
libmmcamera_ov5640_la_CFLAGS += -include stdint.h
libmmcamera_ov5640_la_CFLAGS += -DVFE_32

libmmcamera_ov5640_la_CFLAGS += -I $(CURR_PATH)/includes
libmmcamera_ov5640_la_CFLAGS += -I $(MM2_PATH)/includes

#TODO:
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

libmmcamera_ov5640_la_SOURCES = ov5640_lib.c

#LOCAL_SHARED_LIBRARIES := libcutils
libmmcamera_ov5640_la_LIBADD = -lcutils
libmmcamera_ov5640_la_LDFLAGS = -shared
#libmmcamera_ov5640_la_LIBADD := -ldl
