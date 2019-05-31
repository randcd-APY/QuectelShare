LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := pktlogconf
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../uapi/linux
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../target/inc
LOCAL_SRC_FILES := pktlogconf.c
include $(BUILD_EXECUTABLE)
