# ---------------------------------------------------------------------------
#                  Make the shared library (mmcamera_eztune_module)
# ---------------------------------------------------------------------------
LOCAL_PATH = $(top_srcdir)/media-controller/modules/pproc-new/c2d
PPROC_MODULE_PATH = $(LOCAL_PATH)/../../pproc-new
QC_PROP_ROOT = $(WORKSPACE)/camera/services
SRC_CAMERA_HAL_DIR = $(WORKSPACE)/camera/lib
LOCAL_MMCAMERA_PATH = $(QC_PROP_ROOT)/mm-camera/mm-camera2

LOCAL_C_INCLUDES = -I$(LOCAL_PATH)
LOCAL_C_INCLUDES += -I$(PPROC_MODULE_PATH)
LOCAL_C_INCLUDES += -I$(PPROC_MODULE_PATH)/buf_mgr
LOCAL_C_INCLUDES += -I$(PPROC_MODULE_PATH)/utils
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/server-tuning/tuning/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/port
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/object
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/event
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/controller
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/module
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/fdleak
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/pproc-new/base
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/module/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/chromatix/$(CHROMATIX_VERSION)
LOCAL_C_INCLUDES += -I$(MM_CAMERASDK_PATH)/sensor/includes/
LOCAL_C_INCLUDES += -I$(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
LOCAL_C_INCLUDES += -I$(SRC_CAMERA_HAL_DIR)/QCamera2/stack/common

LOCAL_CFLAGS = -Werror

LOCAL_SRC_FILES = eztune_module.c eztune_port.c eztune_utils.c

LOCAL_LDFLAGS = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

LOCAL_LIBADD = -lcutils -ldl ../../../mct/libmmcamera2_mct.la ../base/libmmcamera_ppbase_module.la -lmmcamera_dbg -llog

lib_LTLIBRARIES = libmmcamera_eztune_module.la
libmmcamera_eztune_module_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera_eztune_module_la_LIBADD = $(LOCAL_LIBADD) 
libmmcamera_eztune_module_la_LDFLAGS = $(LOCAL_LDFLAGS)
libmmcamera_eztune_module_la_CFLAGS =  $(LOCAL_C_INCLUDES) $(LOCAL_CFLAGS)
