#======================================================================
#makefile for libmmcamera2_isp_modules.so form mm-camera2
#======================================================================
ACLOCAL_AMFLAGS = -I m4
LOCAL_PATH = $(top_srcdir)/media-controller/mct
QC_PROP_ROOT = $(WORKSPACE)/camera/services
LOCAL_MMCAMERA_PATH = $(QC_PROP_ROOT)/mm-camera/mm-camera2

LOCAL_CFLAGS  = -Werror

LOCAL_C_INCLUDES  = -I $(LOCAL_PATH)
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/includes/

LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/server-tuning/tuning/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/includes/

LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/controller/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/event/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/module/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/object/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/port/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/fdleak

LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes/
LOCAL_C_INCLUDES += -I  $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

LOCAL_C_INCLUDES += -I $(COMMON_INCDIR)/mm-camera/
LOCAL_C_INCLUDES += -I $(MM_CAMERASDK_PATH)/sensor/includes/

LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/common/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/module_algo/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/module_algo/la_algo/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/module_algo/ltm_algo/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/module_algo/gtm_algo/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/module_algo/tintless_algo/
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/
LOCAL_C_INCLUDES += -I $(WORKSPACE)/camera/lib/QCamera2/stack/common

if ENABLE_msm8996
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/isp47
endif
if ENABLE_msm8998
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/isp48
endif
if ENABLE_msm8953
LOCAL_C_INCLUDES += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/isp2/module/isp42
endif

LOCAL_SRC_FILES        = isp_module.c \
                          isp_port.c \
                          isp_handler.c \
                          isp_resource.c \
                          isp_trigger_thread.c \
                          isp_hw_update_thread.c \
                          isp_hw_update_util.c \
                          isp_parser_thread.c \
                          isp48/isp_pipeline48.c \
                          isp48/isp_pipeline48_parser.c \
                          isp47/isp_pipeline47.c \
                          isp47/isp_pipeline47_parser.c \
                          isp46/isp_pipeline46.c \
                          isp46/isp_pipeline46_parser.c \
                          isp44/isp_pipeline44.c \
                          isp40/isp_pipeline40.c \
                          isp40/isp_pipeline40_parser.c \
                          isp41/isp_pipeline41.c \
                          isp41/isp_pipeline41_parser.c \
                          isp42/isp_pipeline42.c \
                          isp42/isp_pipeline42_parser.c \
                          isp32/isp_pipeline32.c \
                          isp44/isp_pipeline44_parser.c \
                          module_algo/isp_algo.c \
                          module_algo/la_algo/la40_algo.c \
                          module_algo/ltm_algo/ltm44_algo.c \
                          module_algo/ltm_algo/autoltm.c \
                          module_algo/gtm_algo/gtm46_algo.c \
                          module_algo/tintless_algo/tintless40_algo.c \
                          isp_util.c \
                          isp_stats_buf_mgr.c
AM_CPPFLAGS = -include cutils/trace.h
AM_CPPFLAGS += -include cutils/properties.h

LOCAL_LDFLAGS = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

LOCAL_LIBADD = -ldl -lcutils -lmmcamera_dbg -llog
LOCAL_LIBADD += ../../../mct/libmmcamera2_mct.la

lib_LTLIBRARIES = libmmcamera2_isp_modules.la
libmmcamera2_isp_modules_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera2_isp_modules_la_LDFLAGS = $(LOCAL_LDFLAGS)
libmmcamera2_isp_modules_la_LIBADD = $(LOCAL_LIBADD)
libmmcamera2_isp_modules_la_CPPFLAGS =  $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) $(AM_CPPFLAGS)
