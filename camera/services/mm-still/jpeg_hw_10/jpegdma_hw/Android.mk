ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#===============================================================================
#             Deploy the headers that can be exposed
#===============================================================================

LOCAL_COPY_HEADERS_TO := mm-still/jpeg_hw_10
LOCAL_COPY_HEADERS += inc/jpegdma_lib.h
LOCAL_COPY_HEADERS += inc/jpegdma_lib_common.h
LOCAL_COPY_HEADERS += inc/jpegdma_app_util_mmap.h

#===============================================================================
#             Compile Shared library libjpegdmahw.so
#===============================================================================
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../common
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc

ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
endif

ifeq ($(strip $(TARGET_USES_ION)),true)
LOCAL_CFLAGS += -DUSE_ION
endif

ifeq ($(strip $(NEW_LOG_API)),true)
LOCAL_CFLAGS += -DNEW_LOG_API
endif

LOCAL_CFLAGS += -Werror

LOCAL_SRC_FILES := src/jpegdma_lib.c
LOCAL_SRC_FILES += src/jpegdma_lib_hw.c
LOCAL_SRC_FILES += src/jpegdma_app_util_mmap.c

LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_MODULE :=libjpegdmahw
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

#LOCAL_CLANG := false

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_32_BIT_ONLY := true
include $(BUILD_SHARED_LIBRARY)

endif #BUILD_TINY_ANDROID
