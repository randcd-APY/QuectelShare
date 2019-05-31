
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
        $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := BTLogKit
LOCAL_CERTIFICATE := platform

LOCAL_JNI_SHARED_LIBRARIES := libqlogging_jni

LOCAL_REQUIRED_MODULES := bluetooth.default
LOCAL_MULTILIB := 32

LOCAL_PROGUARD_ENABLED := disabled

ifeq ($(TARGET_BUILD_VARIANT),userdebug)
    include $(BUILD_PACKAGE)
    include $(call all-makefiles-under,$(LOCAL_PATH))
endif
