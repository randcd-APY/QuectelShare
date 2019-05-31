#ACLOCAL_AMFLAGS = -I m4

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/pproc-new
MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2

#libmmcamera2_c2d_module.so
lib_LTLIBRARIES = libmmcamera2_c2d_module.la

libmmcamera2_c2d_module_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera2_c2d_module_la_CFLAGS += -DMSM_CAMERA_GCC
libmmcamera2_c2d_module_la_CFLAGS += -fPIC
libmmcamera2_c2d_module_la_CFLAGS += -include stdint.h
libmmcamera2_c2d_module_la_CFLAGS += -DLOGE=ALOGE
libmmcamera2_c2d_module_la_CFLAGS += -include utils/Log.h

libmmcamera2_c2d_module_la_CFLAGS += -I $(CURR_PATH)
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/includes

libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/includes
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/includes
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tuning
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug

libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/pproc-new/buf_mgr
libmmcamera2_c2d_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix/0301
libmmcamera2_c2d_module_la_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/

#TODO::
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/include/adreno/
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/include/adreno200/

#TODO:
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#LOCAL_C_INCLUDES += \
# $(LOCAL_PATH)/../../../../../../../../hardware/qcom/camera/QCamera2/stack/common

libmmcamera2_c2d_module_la_SOURCES = c2d_module.c
libmmcamera2_c2d_module_la_SOURCES += c2d_module_events.c
libmmcamera2_c2d_module_la_SOURCES += c2d_module_util.c
libmmcamera2_c2d_module_la_SOURCES += c2d_port.c
libmmcamera2_c2d_module_la_SOURCES += c2d_thread.c
libmmcamera2_c2d_module_la_SOURCES += c2d_hardware.c
#libmmcamera2_c2d_module_la_SOURCES += cpp_hw_params.c
libmmcamera2_c2d_module_la_SOURCES += ../buf_mgr/pp_buf_mgr.c
libmmcamera2_c2d_module_la_SOURCES += c2d.c

#LOCAL_SHARED_LIBRARIES := libcutils liboemcamera libdl
libmmcamera2_c2d_module_la_LDFLAGS = -shared
libmmcamera2_c2d_module_la_LIBADD = $(BUILD_PATH)/media-controller/mct/liboemcamera.la
libmmcamera2_c2d_module_la_LIBADD += -ldl -lcutils -llog #-loemcamera

dirs =
SUBDIRS = $(dirs)
