LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := cnss-daemon
LOCAL_MODULE_TAGS := optional

LOCAL_CLANG := true
LOCAL_C_INCLUDES := \
	$(TARGET_OUT_HEADERS)/qmi-framework/inc \
	$(TARGET_OUT_HEADERS)/qmi/inc \
	$(TARGET_OUT_HEADERS)/qmi-framework/qcci/inc \
	$(TARGET_OUT_HEADERS)/external/libnl/include

ifeq ($(strip $(TARGET_USES_ICNSS_QMI)),true)
LOCAL_CFLAGS += -DICNSS_QMI

LOCAL_C_INCLUDES += \
	$(TARGET_OUT_HEADERS)/libmdmdetect/inc \
	$(TARGET_OUT_HEADERS)/libperipheralclient/inc
endif

LOCAL_LD_FLAGS := -Wl,--hash-style=sysv

LOCAL_SRC_FILES := \
	main.c \
	nl_loop.c \
	debug.c \
	cnss_qmi_client.c \
	wireless_lan_proxy_service_v01.c \
	cnss_gw_update.c

ifeq ($(strip $(TARGET_USES_ICNSS_QMI)),true)
LOCAL_SRC_FILES += wlfw_qmi_client.c \
		wlan_firmware_service_v01.c
endif

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libqmi_cci \
	libqmi_common_so \
	libnl

ifeq ($(strip $(TARGET_USES_ICNSS_QMI)),true)
LOCAL_SHARED_LIBRARIES += \
	libperipheral_client \
	libmdmdetect \
	libqmiservices
endif

LOCAL_CFLAGS += \
	-DCONFIG_DEBUG \
	-DCONFIG_DEBUG_LOGCAT

include $(BUILD_EXECUTABLE)
