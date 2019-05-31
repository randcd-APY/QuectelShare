#ACLOCAL_AMFLAGS = -I m4

MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2

#libmmcamera2_iface_modules
lib_LTLIBRARIES = libmmcamera2_iface_modules.la

libmmcamera2_iface_modules_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera2_iface_modules_la_CFLAGS += -DMSM_CAMERA_GCC
libmmcamera2_iface_modules_la_CFLAGS += -fPIC
libmmcamera2_iface_modules_la_CFLAGS += -DAF_2X13_FILTER_SUPPORT
libmmcamera2_iface_modules_la_CFLAGS += -include stdint.h
libmmcamera2_iface_modules_la_CFLAGS += -D_ANDROID_

libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/includes
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/includes


libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/includes
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/controller
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tuning
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug

libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/includes
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/includes


libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/isp
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/isp/core
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/isp/hw
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/isp/hw/includes
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/isp/hw/axi
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/isp/hw/pix
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/isp/hw/pix/pix40
libmmcamera2_iface_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/isp/hw/pix/modules
libmmcamera2_iface_modules_la_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/
#TODO:
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#LOCAL_C_INCLUDES += \
# $(LOCAL_PATH)/../../../../../../../../hardware/qcom/camera/QCamera2/stack/common

libmmcamera2_iface_modules_la_SOURCES = ispif_util.c
libmmcamera2_iface_modules_la_SOURCES += ispif.c
libmmcamera2_iface_modules_la_SOURCES += module_ispif.c
libmmcamera2_iface_modules_la_SOURCES += port_ispif.c

#LOCAL_SHARED_LIBRARIES := liblog libcutils liboemcamera libmmcamera2_isp_modules
libmmcamera2_iface_modules_la_LDFLAGS = -shared
libmmcamera2_iface_modules_la_LIBADD = -ldl -lrt
libmmcamera2_iface_modules_la_LIBADD += $(BUILD_PATH)/media-controller/mct/liboemcamera.la
#libmmcamera2_iface_modules_la_LIBADD += $(MM2_PATH)/media-controller/modules/isp/libmmcamera2_isp_modules.la

dirs =
SUBDIRS = $(dirs)

if USE_DLOG
libmmcamera2_iface_modules_la_CFLAGS += -DUSE_DLOG @DLOG_CFLAGS@
libmmcamera2_iface_modules_la_LIBADD += @DLOG_LIBS@
endif
