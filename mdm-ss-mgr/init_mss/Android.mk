LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := init_mss
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := init_mss.c
LOCAL_CFLAGS := -DSLEEP_INDEFINITE

include $(BUILD_EXECUTABLE)

