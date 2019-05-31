LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    com_android_qlogging_sendIntent.cpp

LOCAL_C_INCLUDES += \
    $(JNI_H_INCLUDE) \
    system/bt/include \
    system/bt/stack/include \

LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libnativehelper \
    libcutils \
    libutils \
    liblog \
    libhardware

LOCAL_MULTILIB := 32

#LOCAL_CFLAGS += -O0 -g

LOCAL_MODULE := libqlogging_jni
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
