#ACLOCAL_AMFLAGS = -I m4

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/pproc-new
MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2

#libmmcamera2_wnr_module.so
lib_LTLIBRARIES = libmmcamera2_wnr_module.la

libmmcamera2_wnr_module_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera2_wnr_module_la_CFLAGS += -DMSM_CAMERA_GCC
libmmcamera2_wnr_module_la_CFLAGS += -fPIC
libmmcamera2_wnr_module_la_CFLAGS  += -include stdint.h
libmmcamera2_wnr_module_la_CFLAGS += -DLOGE=ALOGE
libmmcamera2_wnr_module_la_CFLAGS += -include utils/Log.h

libmmcamera2_wnr_module_la_CFLAGS += -I $(CURR_PATH)
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/includes

libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/includes
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tuning
libmmcamera2_wnr_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug
libmmcamera2_wnr_module_la_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/

#TODO:
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#LOCAL_C_INCLUDES += \
# $(LOCAL_PATH)/../../../../../../../../hardware/qcom/camera/QCamera2/stack/common

libmmcamera2_wnr_module_la_SOURCES = wnr_module.c

#LOCAL_SHARED_LIBRARIES := libcutils liboemcamera
libmmcamera2_wnr_module_la_LDFLAGS = -shared
libmmcamera2_wnr_module_la_LIBADD = -lcutils -llog $(BUILD_PATH)/media-controller/mct/liboemcamera.la
