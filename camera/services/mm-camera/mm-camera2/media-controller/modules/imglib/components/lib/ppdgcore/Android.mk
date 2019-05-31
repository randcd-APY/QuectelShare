CURRENT_LOCAL_PATH := $(LOCAL_PATH)

LOCAL_PATH := $(call my-dir)
LOCAL_DIR_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LE_PREBUILD_LIB ?= false
LOCAL_MMCAMERA_PATH  := $(QC_PROP_ROOT)/mm-camera/mm-camera2

COMMON_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/include/
COMMON_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/utils/
COMMON_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/lib/

COMMON_CFLAGS := -Wno-non-virtual-dtor -fno-exceptions
COMMON_CFLAGS += -D_ANDROID_


LOCAL_MODULE           := libmmcamera_ppdgcore
LOCAL_CLANG := false
LOCAL_32_BIT_ONLY      := true
LOCAL_MODULE_TAGS      := optional

LOCAL_C_INCLUDES += $(COMMON_C_INCLUDES)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcom/display
LOCAL_C_INCLUDES += system/core/include
LOCAL_C_INCLUDES += $(LOCAL_MMCAMERA_PATH)/includes/
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES += $(SRC_CAMERA_HAL_DIR)/QCamera2/stack/common

LOCAL_CFLAGS := $(COMMON_CFLAGS)
LOCAL_CFLAGS  += -include $(LOCAL_IMGLIB_PATH)/components/include/img_dbg.h

ifeq ($(LE_PREBUILD_LIB),true)
LOCAL_CFLAGS += --std=c++11
endif

LOCAL_SRC_FILES := QCameraPPDgCore.cpp

LOCAL_SHARED_LIBRARIES += libdl libcutils liblog libgui libEGL libGLESv2 \
libhardware libqdMetaData libutils libui libmmcamera_dbg libmmcamera_imglib

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_MODULE_TAGS      := optional debug
LOCAL_MODULE_OWNER := qti
LOCAL_32_BIT_ONLY  := true
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

LOCAL_PATH := $(CURRENT_LOCAL_PATH)
