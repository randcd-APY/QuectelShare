# ---------------------------------------------------------------------------
#                  Make the shared library (libmmcamera_cpp)
# ---------------------------------------------------------------------------
ACLOCAL_AMFLAGS = -I m4
LOCAL_PATH = $(top_srcdir)/media-controller/modules/pproc-new/cpp
PPROC_MODULE_PATH = $(LOCAL_PATH)/../../pproc-new
QC_PROP_ROOT = $(WORKSPACE)/camera/services
SRC_CAMERA_HAL_DIR = $(WORKSPACE)/camera/lib
LOCAL_MMCAMERA_PATH = $(QC_PROP_ROOT)/mm-camera/mm-camera2
LOCAL_CHROMATIX_PATH = $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

LOCAL_C_INCLUDES = -I$(LOCAL_PATH)
LOCAL_C_INCLUDES += -I$(PPROC_MODULE_PATH)
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/server-tuning/tuning/
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
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/pproc-new/buf_mgr
LOCAL_C_INCLUDES += -I$(LOCAL_CHROMATIX_PATH)
LOCAL_C_INCLUDES += -I$(SRC_CAMERA_HAL_DIR)/QCamera2/stack/common
LOCAL_C_INCLUDES += -I$(COMMON_INCDIR)/mm-camera-interface

LOCAL_CFLAGS = -Werror 

if IS_CHROMATIX_VERSION_0309
LOCAL_CFLAGS += -DCHROMATIX_309
endif
if IS_CHROMATIX_VERSION_0310
LOCAL_CFLAGS += -DCHROMATIX_310
endif
if ENABLE_msm8996
LOCAL_CFLAGS += -DCAMERA_USE_CHROMATIX_HW_WNR_TYPE
endif
if ENABLE_msm8998
LOCAL_CFLAGS += -DCAMERA_USE_CHROMATIX_HW_WNR_TYPE
endif
LOCAL_SRC_FILES = cpp_module.c
LOCAL_SRC_FILES += cpp_module_events.c
LOCAL_SRC_FILES += cpp_module_util.c
LOCAL_SRC_FILES += cpp_port.c
LOCAL_SRC_FILES += cpp_thread.c
LOCAL_SRC_FILES += cpp_hardware.c
LOCAL_SRC_FILES += cpp_hw_params.c
LOCAL_SRC_FILES += cpp_hw_params_1_8.c
LOCAL_SRC_FILES += cpp_tnr_params.c
LOCAL_SRC_FILES += cpp_hw_params_1_10.c
LOCAL_SRC_FILES += cpp_hw_params_1_12.c
LOCAL_SRC_FILES += cpp_pbf_params.c
LOCAL_SRC_FILES += cpp_wnr_params.c
LOCAL_SRC_FILES += cpp_hw_params_1_5.c

if IS_CHROMATIX_VERSION_0308E
LOCAL_SRC_FILES += cpp_hw_params_utils_ext.c
endif

LOCAL_SRC_FILES += cpp_chromatix_int.c

LOCAL_LIBADD = -lcutils -lmmcamera_dbg
LOCAL_LIBADD += ../../../mct/libmmcamera2_mct.la ../buf_mgr/libmmcamera2_pp_buf_mgr.la
LOCAL_LIBADD += -llog

LOCAL_LDFLAGS = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

lib_LTLIBRARIES = libmmcamera2_cpp_module.la

libmmcamera2_cpp_moduledir = $(rootdir)/lib/firmware
dist_libmmcamera2_cpp_module_DATA  = firmware/cpp_firmware_v1_1_1.fw \
				      firmware/cpp_firmware_v1_1_6.fw \
     				      firmware/cpp_firmware_v1_2_0.fw \
				      firmware/cpp_firmware_v1_4_0.fw \
				      firmware/cpp_firmware_v1_5_0.fw \
				      firmware/cpp_firmware_v1_5_1.fw \
				      firmware/cpp_firmware_v1_5_2.fw \
				      firmware/cpp_firmware_v1_6_0.fw \
				      firmware/cpp_firmware_v1_8_0.fw \
				      firmware/cpp_firmware_v1_10_0.fw \
				      firmware/cpp_firmware_v1_12_0.fw

libmmcamera2_cpp_module_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera2_cpp_module_la_LIBADD = $(LOCAL_LIBADD)
libmmcamera2_cpp_module_la_LDFLAGS = $(LOCAL_LDFLAGS) 
libmmcamera2_cpp_module_la_CFLAGS =  $(LOCAL_C_INCLUDES) $(LOCAL_CFLAGS)
