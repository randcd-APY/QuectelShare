LOCAL_PATH = $(top_srcdir)/media-controller/mct_shim_layer
LOCAL_MMCAMERA_PATH  = $(LOCAL_PATH)/../../../
LOCAL_MMCAMERA2_PATH = $(LOCAL_PATH)/../../../mm-camera2

lib_LTLIBRARIES = libmmcamera2_mct_shimlayer.la

libmmcamera2_mct_shimlayer_la_CPPFLAGS = -DMSM_CAMERA_BIONIC

libmmcamera2_mct_shimlayer_la_CFLAGS = -Werror

libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_PATH)
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/includes/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/services/thread/include
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/bus/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/controller/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/object/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/includes/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/tools/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/event/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/pipeline/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/stream/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/module/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/port/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(LOCAL_MMCAMERA2_PATH)/media-controller/mct/debug/
libmmcamera2_mct_shimlayer_la_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common

libmmcamera2_mct_shimlayer_la_CPPFLAGS += -DDAEMON_PRESENT

libmmcamera2_mct_shimlayer_la_SOURCES = mct_shim_layer.c

libmmcamera2_mct_shimlayer_la_LIBADD = -lcutils -ldl ../mct/libmmcamera2_mct.la \
                                   ../modules/stats/libmmcamera2_stats_modules.la \
                                   ../modules/iface2/libmmcamera2_iface_modules.la \
                                   ../modules/isp2/module/libmmcamera2_isp_modules.la \
                                   ../modules/sensors/libmmcamera2_sensor_modules.la \
                                   ../modules/pproc-new/libmmcamera2_pproc_modules.la \
                                   ../modules/imglib/libmmcamera2_imglib_modules.la \
                                   ../../services/thread/libmmcamera_thread_services.la \
                                   -lmmcamera_dbg

libmmcamera2_mct_shimlayer_la_LDFLAGS  = -shared
