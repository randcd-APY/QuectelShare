IMX135_SENSOR_LIBS_PATH := $(call my-dir)

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx074_preview)
# ---------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_PATH := $(IMX135_SENSOR_LIBS_PATH)
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS:= \
        -DAMSS_VERSION=$(AMSS_VERSION) \
        $(mmcamera_debug_defines) \
        $(mmcamera_debug_cflags)

ifeq ($(VFE_VERS),vfe40)
  LOCAL_CFLAGS += -DVFE_40
endif

ifeq ($(MSM_VERSION),8974)
LOCAL_CFLAGS += -D_FULL_RES_30FPS
else ifeq ($(MSM_VERSION),8226)
LOCAL_CFLAGS += -D_FULL_RES_30FPS
else ifeq ($(MSM_VERSION),8916)
LOCAL_CFLAGS += -D_MSM_BEAR -D_FULL_RES_30FPS
endif

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../sensors/module
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../includes/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../modules/includes/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/tools/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/bus/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/controller/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/event/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/module/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/object/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/pipeline/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/port/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/stream/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../includes/
LOCAL_C_INCLUDES += \
 $(LOCAL_PATH)/../../../../../../../../../../hardware/qcom/camera/QCamera2/stack/common
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../../mm-camerasdk/sensor/includes/

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES        := imx135_lib.c
LOCAL_MODULE           := libmmcamera_imx135
LOCAL_SHARED_LIBRARIES := libcutils liblog

ifeq ($(MM_DEBUG),true)
LOCAL_SHARED_LIBRARIES += liblog
endif

LOCAL_MODULE_OWNER := qcom 
LOCAL_32_BIT_ONLY := true
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
