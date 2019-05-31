#======================================================================
#makefile for libmmcamera2_mct.so from mm-camera2
#======================================================================
ACLOCAL_AMFLAGS = -I m4
LOCAL_PATH = $(top_srcdir)/media-controller/mct
MM_CAMERA_PATH = $(LOCAL_PATH)/../../..

LOCAL_CFLAGS  = -Werror

LOCAL_CPPFLAGS  = -I$(LOCAL_PATH)
LOCAL_CPPFLAGS += -I$(MM_CAMERA_PATH)/mm-camera2/includes/
LOCAL_CPPFLAGS += -I$(MM_CAMERA_PATH)/mm-camera2/media-controller/includes/

#************* MCT headers ************#
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/bus/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/controller/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/event/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/module/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/object/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/pipeline/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/port/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/stream/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/tools/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/debug/
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/debug/fdleak
LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/supermct

#************* HAL headers ************#
LOCAL_CPPFLAGS +=  -I$(CAMERAHAL_PATH)/QCamera2/stack/common

LOCAL_CPPFLAGS += -DMCT_STUCK_FLAG
LOCAL_CPPFLAGS += -DDAEMON_PRESENT

LOCAL_SRC_FILES = \
	debug/server_debug.c \
	supermct/supermct.c \
	object/mct_object.c \
	port/mct_port.c \
	bus/mct_bus.c \
	stream/mct_stream.c \
	module/mct_module.c \
	pipeline/mct_pipeline.c \
	tools/mct_list.c \
	tools/mct_profiler.c \
	tools/mct_util.c \
	tools/mct_queue.c \
	controller/mct_controller.c

LOCAL_LDFLAGS = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

LOCAL_LIBADD = -ldl -llog -lcutils -lmmcamera_dbg -lrt

lib_LTLIBRARIES = libmmcamera2_mct.la
libmmcamera2_mct_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera2_mct_la_LIBADD = $(LOCAL_LIBADD)
libmmcamera2_mct_la_LDFLAGS = $(LOCAL_LDFLAGS)
libmmcamera2_mct_la_CPPFLAGS = $(LOCAL_CPPFLAGS)
libmmcamera2_mct_la_CFLAGS = $(LOCAL_CFLAGS)
