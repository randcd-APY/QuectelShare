# ---------------------------------------------------------------------------
#                  Make the shared library (mmcamera_ppbase_module)
# ---------------------------------------------------------------------------
ACLOCAL_AMFLAGS = -I m4
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
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/fdleak
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/port
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/object
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/event
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/module
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/chromatix/$(CHROMATIX_VERSION)
LOCAL_C_INCLUDES += -I$(SRC_CAMERA_HAL_DIR)/QCamera2/stack/common

LOCAL_CFLAGS = -Werror

LOCAL_LIBADD = -lcutils ../../../mct/libmmcamera2_mct.la -lmmcamera_dbg -llog

LOCAL_LDFLAGS = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

LOCAL_SRC_FILES = base_module.c base_port.c base_module_util.c

lib_LTLIBRARIES = libmmcamera_ppbase_module.la
libmmcamera_ppbase_module_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera_ppbase_module_la_LIBADD = $(LOCAL_LIBADD) 
libmmcamera_ppbase_module_la_LDFLAGS = $(LOCAL_LDFLAGS)
libmmcamera_ppbase_module_la_CFLAGS =  $(LOCAL_C_INCLUDES) $(LOCAL_CFLAGS)
