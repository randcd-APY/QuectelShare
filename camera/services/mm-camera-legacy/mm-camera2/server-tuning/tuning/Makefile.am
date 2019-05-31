CURR_PATH = $(top_srcdir)/mm-camera2/server-tuning/tuning
MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2


lib_LTLIBRARIES = libmmcamera_tuning.la

libmmcamera_tuning_la_CFLAGS  = -Werror

libmmcamera_tuning_la_CXXFLAGS  = -std=c++11 -Werror

libmmcamera_tuning_la_SOURCES = eztune_interface.cpp \
                                eztune_protocol.cpp \
                                eztune_process.cpp \
                                eztune_cam_adapter.cpp \
                                mmcam_log_utils.cpp \
                                mmcam_socket_utils.cpp \
                                eztune_items_0301.c


libmmcamera_tuning_la_CXXFLAGS += $(CURR_PATH)
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix/0301
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuators/0301/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuator_libs/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/includes/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/bus/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/controller/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/object/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/includes/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/includes/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/tools/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/event/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/stream/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/debug/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/module/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/port/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/module/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/includes/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuators/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/aec/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/awb/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/af/
libmmcamera_tuning_la_CXXFLAGS += -I$(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/
libmmcamera_tuning_la_CXXFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline

#libmmcamera_tuning_la_CXXFLAGS += -I $(WORKSPACE)/camera/lib-legacy/server/statsproc/aec/

#libmmcamera_tuning_la_CXXFLAGS += $(LOCAL_FASTCV_PATH)/inc/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix/0301
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuators/0301/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuator_libs/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/includes/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/controller/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/includes/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/includes/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/module/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/includes/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuators/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/aec/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/awb/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/af/
libmmcamera_tuning_la_CFLAGS += -I$(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/
libmmcamera_tuning_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline

libmmcamera_tuning_la_LIBADD = -lmmcamera2_stats_modules
libmmcamera_tuning_la_LIBADD += -lmmcamera2_iface_modules
libmmcamera_tuning_la_LIBADD += -lmmcamera2_isp_modules
libmmcamera_tuning_la_LIBADD += -lmmcamera2_sensor_modules
libmmcamera_tuning_la_LIBADD += -lmmcamera2_pproc_modules
libmmcamera_tuning_la_LIBADD += -lmmcamera2_imglib_modules
libmmcamera_tuning_la_LIBADD += -lmm-qcamera
libmmcamera_tuning_la_LIBADD += -llog -ldl -lcutils -lfastcvopt -lc
libmmcamera_tuning_la_LIBADD += -loemcamera
libmmcamera_tuning_la_LDFLAGS  = -shared
