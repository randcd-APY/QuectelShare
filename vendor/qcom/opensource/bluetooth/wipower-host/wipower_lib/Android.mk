
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := android.wipower
LOCAL_SRC_FILES := $(call all-java-files-under,.)
LOCAL_SRC_FILES += android/wipower/IWipowerManagerCallback.aidl
LOCAL_SRC_FILES += android/wipower/IWipower.aidl
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := android.wipower.xml
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

