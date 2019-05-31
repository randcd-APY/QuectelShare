# ---------------------------------------------------------------------------
#                  Make the shared library (libmmcamera2_pp_buf_mgr)
# ---------------------------------------------------------------------------
ACLOCAL_AMFLAGS = -I m4
LOCAL_PATH = $(top_srcdir)/media-controller/modules/pproc-new/buf_mgr
PPROC_MODULE_PATH = $(LOCAL_PATH)/../../pproc-new
QC_PROP_ROOT = $(WORKSPACE)/camera/services
SRC_CAMERA_HAL_DIR = $(WORKSPACE)/camera/lib
LOCAL_MMCAMERA_PATH = $(QC_PROP_ROOT)/mm-camera/mm-camera2
LOCAL_CHROMATIX_PATH = $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

LOCAL_C_INCLUDES = -I$(LOCAL_PATH)
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
LOCAL_C_INCLUDES += -I$(SRC_CAMERA_HAL_DIR)/QCamera2/stack/common
LOCAL_C_INCLUDES += -I$(COMMON_INCDIR)/mm-camera-interface

LOCAL_CFLAGS  = -Werror

LOCAL_SRC_FILES = pp_buf_mgr.c

LOCAL_LDFLAGS = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

LOCAL_LIBADD = -lcutils -lmmcamera_dbg
LOCAL_LIBADD += -llog
LOCAL_LIBADD += ../../../mct/libmmcamera2_mct.la

lib_LTLIBRARIES = libmmcamera2_pp_buf_mgr.la
libmmcamera2_pp_buf_mgr_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera2_pp_buf_mgr_la_LDFLAGS = $(LOCAL_LDFLAGS)
libmmcamera2_pp_buf_mgr_la_LIBADD = $(LOCAL_LIBADD)
libmmcamera2_pp_buf_mgr_la_CFLAGS =  $(LOCAL_C_INCLUDES) $(LOCAL_CFLAGS) 
