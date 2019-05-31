LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := diagrebootapp
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := $(TARGET_OUT_HEADERS)/diag/include \
                 $(TARGET_OUT_HEADERS)/common/inc \
                 $(TARGET_OUT_HEADERS)/qmi/inc

LOCAL_SRC_FILES := registerReboot.c
LOCAL_SHARED_LIBRARIES += libdiag libcutils

include $(BUILD_EXECUTABLE)

