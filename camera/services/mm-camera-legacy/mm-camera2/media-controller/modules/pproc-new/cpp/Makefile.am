#ACLOCAL_AMFLAGS = -I m4

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/pproc-new
MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2

#libmmcamera2_cpp_module.so
lib_LTLIBRARIES = libmmcamera2_cpp_module.la

libmmcamera2_cpp_module_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera2_cpp_module_la_CFLAGS += -DMSM_CAMERA_GCC
libmmcamera2_cpp_module_la_CFLAGS += -DARM
libmmcamera2_cpp_module_la_CFLAGS += -fPIC
libmmcamera2_cpp_module_la_CFLAGS += -include stdint.h
libmmcamera2_cpp_module_la_CFLAGS += -include stddef.h
libmmcamera2_cpp_module_la_CFLAGS += -DLOGE=ALOGE
libmmcamera2_cpp_module_la_CFLAGS += -include utils/Log.h


#libmmcamera2_cpp_module_la_CFLAGS += -pedantic
#if MSM8909
libmmcamera2_cpp_module_la_CFLAGS += -DCAMERA_FEATURE_WNR_SW
#endif

libmmcamera2_cpp_module_la_CFLAGS += -I .
libmmcamera2_cpp_module_la_CFLAGS += -I $(CURR_PATH)
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/server-tuning/tuning
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/includes
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/includes
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/includes
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tuning
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug
libmmcamera2_cpp_module_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix/0301
libmmcamera2_cpp_module_la_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/

libmmcamera2_cpp_moduledir = $(rootdir)/lib/firmware
dist_libmmcamera2_cpp_module_DATA  = firmware/cpp_firmware_v1_1_1.fw \
                                     firmware/cpp_firmware_v1_1_6.fw \
                                     firmware/cpp_firmware_v1_2_0.fw \
                                     firmware/cpp_firmware_v1_4_0.fw

libmmcamera2_cpp_module_la_SOURCES = cpp_module.c
libmmcamera2_cpp_module_la_SOURCES += cpp_module_events.c
libmmcamera2_cpp_module_la_SOURCES += cpp_module_util.c
libmmcamera2_cpp_module_la_SOURCES += cpp_port.c
libmmcamera2_cpp_module_la_SOURCES += cpp_thread.c
libmmcamera2_cpp_module_la_SOURCES += cpp_hardware.c
libmmcamera2_cpp_module_la_SOURCES += cpp_hw_params.c

#LOCAL_SHARED_LIBRARIES := libcutils liboemcamera
libmmcamera2_cpp_module_la_LDFLAGS = -shared
libmmcamera2_cpp_module_la_LDFLAGS += -ldl -lrt -llog -lcutils
libmmcamera2_cpp_module_la_LIBADD = $(BUILD_PATH)/media-controller/mct/liboemcamera.la

dirs =
SUBDIRS = $(dirs)

