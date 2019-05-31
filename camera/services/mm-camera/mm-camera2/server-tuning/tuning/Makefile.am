LOCAL_PATH = $(top_srcdir)/server-tuning/tuning
MM_CAMERA_PATH = $(LOCAL_PATH)/../../../mm-camera2

lib_LTLIBRARIES = libmmcamera_tuning.la

libmmcamera_tuning_la_CFLAGS  = -Werror

libmmcamera_tuning_la_CXXFLAGS  = -std=c++11 -Werror

libmmcamera_tuning_la_CPPFLAGS =

if BOARD_QTI_CAMERA_V2
libmmcamera_tuning_la_CPPFLAGS += -Wno-literal-suffix
endif

if IS_CHROMATIX_VERSION_0309
libmmcamera_tuning_la_CPPFLAGS += -DCHROMATIX_309
endif

if IS_CHROMATIX_VERSION_0310
libmmcamera_tuning_la_CPPFLAGS += -DCHROMATIX_310
endif

if CAMERA_USE_CHROMATIX_HW_WNR_TYPE
libmmcamera_tuning_la_CPPFLAGS += -DCAMERA_USE_CHROMATIX_HW_WNR_TYPE
endif

if USE_FASTCV_OPT
libmmcamera_tuning_la_CPPFLAGS += -DUSE_FASTCV_OPT
endif

libmmcamera_tuning_la_SOURCES = eztune_interface.cpp   \
                  eztune_protocol.cpp    \
                  eztune_process.cpp     \
                  eztune_cam_adapter.cpp \
                  mmcam_log_utils.cpp    \
                  mmcam_socket_utils.cpp


libmmcamera_tuning_la_CPPFLAGS += -I $(LOCAL_PATH)
libmmcamera_tuning_la_CPPFLAGS += -I $(LOCAL_PATH)/../includes/
libmmcamera_tuning_la_CPPFLAGS += -I $(LOCAL_PATH)/../lookup/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/includes/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/includes/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/bus/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/controller/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/object/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/includes/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/tools/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/event/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/pipeline/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/stream/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/module/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/port/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/debug/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/debug/fdleak
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/pproc-new/eztune
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/sensors/module/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/sensors/sensors/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/sensors/actuators/module/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/asd/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/aec/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/awb/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/af/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/include/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/platform/linux/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/includes/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_tuning_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_tuning_la_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common/

libmmcamera_tuning_la_LIBADD  = ../lookup/libmmcamera_tuning_lookup.la
libmmcamera_tuning_la_LIBADD += ../../media-controller/mct/libmmcamera2_mct.la
libmmcamera_tuning_la_LIBADD += ../../media-controller/modules/stats/libmmcamera2_stats_modules.la
libmmcamera_tuning_la_LIBADD += ../../media-controller/modules/iface2/libmmcamera2_iface_modules.la
libmmcamera_tuning_la_LIBADD += ../../media-controller/modules/isp2/module/libmmcamera2_isp_modules.la
libmmcamera_tuning_la_LIBADD += ../../media-controller/modules/sensors/libmmcamera2_sensor_modules.la
libmmcamera_tuning_la_LIBADD += ../../media-controller/modules/pproc-new/libmmcamera2_pproc_modules.la
libmmcamera_tuning_la_LIBADD += ../../media-controller/modules/imglib/libmmcamera2_imglib_modules.la
libmmcamera_tuning_la_LIBADD += ../../media-controller/modules/pproc-new/eztune/libmmcamera_eztune_module.la
libmmcamera_tuning_la_LIBADD += -lmm-qcamera
libmmcamera_tuning_la_LIBADD += -llog -ldl -lcutils

if USE_FASTCV_OPT
libmmcamera_tuning_la_LIBADD += -lfastcvopt
endif
libmmcamera_tuning_la_LIBADD += -lmmcamera_dbg

libmmcamera_tuning_la_LDFLAGS  = -shared
libmmcamera_tuning_la_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
libmmcamera_tuning_la_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap
