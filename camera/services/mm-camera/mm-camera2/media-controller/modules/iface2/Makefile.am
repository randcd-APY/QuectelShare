#======================================================================
#makefile for libmmcamera2_iface_modules.so form mm-camera2
#======================================================================
ACLOCAL_AMFLAGS = -I m4
LOCAL_PATH = $(top_srcdir)/media-controller/modules/iface2
LOCAL_MMCAMERA_PATH = $(LOCAL_PATH)/../../../../mm-camera2

LOCAL_CPPFLAGS = -I$(LOCAL_PATH)
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/includes/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/includes/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/controller/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/event/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/module/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/object/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/port/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/fdleak
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/iface2/hvx/
LOCAL_CPPFLAGS += -I$(MM_CAMERASDK_PATH)/sensor/includes/
LOCAL_CPPFLAGS += -I$(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
LOCAL_CPPFLAGS += -I$(MM_CAMERASDK_PATH)/hvx/
LOCAL_CPPFLAGS += -I$(MM_CAMERASDK_PATH)/hvx/lib/zzHDR/
LOCAL_CPPFLAGS += -I$(CAMERAHAL_PATH)/QCamera2/stack/common

LOCAL_CFLAGS = -Werror
LOCAL_CPPFLAGS += -DCDS_FEATURE_ENABLE=1

LOCAL_SRC_FILES = \
	ispif/iface_ispif.c \
	hvx/iface_hvx.c \
	axi/iface_axi_util.c \
	axi/iface_axi_thread.c \
	axi/iface_axi.c \
	port_iface.c \
	iface.c \
	module_iface.c \
	iface_session_thread.c \
	iface_util.c \
	iface_buf_mgr.c

LOCAL_LDFLAGS = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

LOCAL_LIBADD = -llog -lcutils -ldl -lmmcamera_dbg
LOCAL_LIBADD += ../../mct/libmmcamera2_mct.la
LOCAL_LIBADD += ../isp2/module/libmmcamera2_isp_modules.la

pkgincludedir = $(includedir)/mm_camera_interface
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(LOCAL_PATH)/hvx/hvx_stub.h

lib_LTLIBRARIES = libmmcamera2_iface_modules.la
libmmcamera2_iface_modules_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera2_iface_modules_la_LDFLAGS = $(LOCAL_LDFLAGS)
libmmcamera2_iface_modules_la_LIBADD = $(LOCAL_LIBADD)
libmmcamera2_iface_modules_la_CPPFLAGS = $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)
