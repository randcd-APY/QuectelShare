truly_ov8856_EEPROM_LIBS_PATH := $(call my-dir)

# ---------------------------------------------------------------------------
#        Make the shared library (libmmcamera_truly_ov8856_eeprom.so)
# ---------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_PATH := $(truly_ov8856_EEPROM_LIBS_PATH)
LOCAL_MODULE_TAGS := optional
LOCAL_LDFLAGS := $(mmcamera_debug_lflags)

LOCAL_CFLAGS := -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../chromatix/0301
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../actuator_libs/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../actuators/0301
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../eeprom
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../module
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/tools
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/bus
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/controller
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/event
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/module
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/object
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/pipeline
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/port
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../mct/stream
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../includes
LOCAL_C_INCLUDES += \
 $(LOCAL_PATH)/../../../../../../../../../../hardware/qcom/camera/QCamera2/stack/common
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES:= truly_ov8856_eeprom.c
LOCAL_MODULE           := libmmcamera_truly_ov8856_eeprom
LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

ifeq ($(32_BIT_FLAG), true)
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_SHARED_LIBRARY)
