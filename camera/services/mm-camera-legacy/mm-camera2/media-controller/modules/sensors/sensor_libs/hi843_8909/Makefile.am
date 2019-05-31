#libmmcamera_hi843_8909.so

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/sensors
MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2

lib_LTLIBRARIES = libmmcamera_hi843_8909.la

libmmcamera_hi843_8909_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera_hi843_8909_la_CFLAGS += -fPIC
libmmcamera_hi843_8909_la_CFLAGS += -include stdint.h
libmmcamera_hi843_8909_la_CFLAGS += -DVFE_32

libmmcamera_hi843_8909_la_CFLAGS += -I $(CURR_PATH)/includes
libmmcamera_hi843_8909_la_CFLAGS += -I $(MM2_PATH)/includes

#TODO:
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

libmmcamera_hi843_8909_la_SOURCES = hi843_8909_lib.c

#LOCAL_SHARED_LIBRARIES := libcutils
libmmcamera_hi843_8909_la_LIBADD = -lcutils
libmmcamera_hi843_8909_la_LDFLAGS = -shared
#libmmcamera_hi843_8909_la_LIBADD := -ldl
