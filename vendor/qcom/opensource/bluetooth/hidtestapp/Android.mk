LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
src_dirs:= src/org/codeaurora/bluetooth/hidtestapp \

LOCAL_SRC_FILES := \
        $(call all-java-files-under, $(src_dirs)) \

LOCAL_PACKAGE_NAME := HidTestApp
LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_ENABLED := disabled

LOCAL_MULTILIB:= 32

include $(BUILD_PACKAGE)


include $(call all-makefiles-under,$(LOCAL_PATH))
