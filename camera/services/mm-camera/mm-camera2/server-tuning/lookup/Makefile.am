LOCAL_PATH = $(top_srcdir)/server-tuning/lookup
MM_CAMERA_PATH = $(LOCAL_PATH)/../../../mm-camera2

lib_LTLIBRARIES = libmmcamera_tuning_lookup.la

libmmcamera_tuning_lookup_la_SOURCES = eztune_lookup.c eztune_items_diag.c eztune_items_$(CHROMATIX_VERSION).c

libmmcamera_tuning_lookup_la_CPPFLAGS  = -I $(LOCAL_PATH)
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(LOCAL_PATH)/../includes/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(LOCAL_PATH)/../tuning/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/chromatix-wrapper/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/includes/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/includes/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/bus/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/controller/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/object/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/includes/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/tools/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/event/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/pipeline/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/stream/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/module/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/port/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/debug/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/debug/fdleak
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/pproc-new/eztune
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/sensors/module/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/sensors/actuators/module/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/asd/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/aec/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/awb/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/af/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/include/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/stats/q3a/platform/linux/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/modules/includes/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_tuning_lookup_la_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common/

if BOARD_QTI_CAMERA_V2
libmmcamera_tuning_lookup_la_CPPFLAGS  += -Wno-error=literal-suffix
endif

libmmcamera_tuning_lookup_la_LIBADD = -llog -ldl -lcutils

if USE_FASTCV_OPT
libmmcamera_tuning_lookup_la_LIBADD += -lfastcvopt
endif

libmmcamera_tuning_lookup_la_LDFLAGS = -shared
libmmcamera_tuning_lookup_la_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
libmmcamera_tuning_lookup_la_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap
