LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
        $(call all-java-files-under, src)
LOCAL_SRC_FILES := $(filter-out src/org/codeaurora/bluetooth/bttestapp/PbapTestActivity.java,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES := $(filter-out src/org/codeaurora/bluetooth/bttestapp/services/IPbapServiceCallback.java,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES := $(filter-out src/org/codeaurora/bluetooth/bttestapp/services/PbapAuthAcitivty.java,$(LOCAL_SRC_FILES))
LOCAL_PACKAGE_NAME := BTTestApp
LOCAL_CERTIFICATE := platform

LOCAL_MODULE_OWNER := qcom

LOCAL_STATIC_JAVA_LIBRARIES := com.android.vcard  android.bluetooth.client.map

LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
