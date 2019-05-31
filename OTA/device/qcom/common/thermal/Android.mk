LOCAL_PATH:= $(call my-dir)

ifeq ($(call is-vendor-board-platform,QCOM),true)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := thermal.c

ifeq ($(call is-board-platform-in-list,msm8998), true)
LOCAL_SRC_FILES += thermal-8998.c
endif

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_CFLAGS += -Wno-unused-parameter -Wno-unused-variable
LOCAL_MODULE := thermal.$(TARGET_DEVICE)
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
endif
