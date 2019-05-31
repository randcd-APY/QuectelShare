# ---------------------------------------------------------------------------
#                  Make the shared library (libmmcamera2_c2d_module)
# ---------------------------------------------------------------------------
ACLOCAL_AMFLAGS = -I m4
LOCAL_PATH = $(top_srcdir)/media-controller/modules/pproc-new/c2d
PPROC_MODULE_PATH = $(LOCAL_PATH)/../../pproc-new
QC_PROP_ROOT = $(WORKSPACE)/camera/services
SRC_CAMERA_HAL_DIR = $(WORKSPACE)/camera/lib
LOCAL_MMCAMERA_PATH = $(QC_PROP_ROOT)/mm-camera/mm-camera2
LOCAL_CHROMATIX_PATH = $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

LOCAL_C_INCLUDES = -I$(LOCAL_PATH)
LOCAL_C_INCLUDES += -I$(PPROC_MODULE_PATH)
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/port
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/object
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/event
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/module
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/fdleak
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/pproc-new/buf_mgr
LOCAL_C_INCLUDES += -I$(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
LOCAL_C_INCLUDES += -I$(COMMON_INCDIR)/adreno/
LOCAL_C_INCLUDES += -I$(COMMON_INCDIR)/adreno200/
LOCAL_C_INCLUDES += -I$(COMMON_INCDIR)/mm-camera-interface
LOCAL_C_INCLUDES += -I$(SRC_CAMERA_HAL_DIR)/QCamera2/stack/common

LOCAL_CFLAGS = -Werror

LOCAL_SRC_FILES = c2d_module.c
LOCAL_SRC_FILES += c2d_module_events.c
LOCAL_SRC_FILES += c2d_module_util.c
LOCAL_SRC_FILES += c2d_port.c
LOCAL_SRC_FILES += c2d_thread.c
LOCAL_SRC_FILES += c2d_hardware.c
#LOCAL_SRC_FILES += cpp_hw_params.c
LOCAL_SRC_FILES += c2d.c

LOCAL_LIBADD = -lcutils ../../../mct/libmmcamera2_mct.la -ldl ../buf_mgr/libmmcamera2_pp_buf_mgr.la -lmmcamera_dbg -llog

LOCAL_LDFLAGS = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

lib_LTLIBRARIES = libmmcamera2_c2d_module.la
libmmcamera2_c2d_module_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera2_c2d_module_la_LIBADD = $(LOCAL_LIBADD)
libmmcamera2_c2d_module_la_LDFLAGS = $(LOCAL_LDFLAGS)
libmmcamera2_c2d_module_la_CFLAGS =  $(LOCAL_C_INCLUDES) $(LOCAL_CFLAGS)
