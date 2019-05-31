################################################################################
# @file vendor/qcom-proprietary/time_services/Android.mk
# @brief Makefile for building the time services library on Android.
################################################################################

###################### Time services Shared library ############################


ifeq ($(call is-board-platform,msm8960),true)

##################### Build for 8960 target QMI version ########################
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

commonIncludes := $(TARGET_OUT_HEADERS)/common/inc
commonIncludes += $(TARGET_OUT_HEADERS)/diag/include

LOCAL_C_INCLUDES := \
	$(commonIncludes) \
	vendor/qcom/opensource/time-services

LOCAL_SRC_FILES:= time_genoff_qmi.c

LOCAL_SHARED_LIBRARIES := \
	libc \
	libcutils

LOCAL_MODULE:= libtime_genoff
LOCAL_MODULE_TAGS := optional eng
LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)

###################### Time services daemon  #################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= time_daemon_qmi.c time_service_v01.c

commonIncludes := $(TARGET_OUT_HEADERS)/common/inc
commonIncludes += $(TARGET_OUT_HEADERS)/diag/include

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(TARGET_OUT_HEADERS)/qmi-framework/inc \
	$(TARGET_OUT_HEADERS)/qmi-framework/qcci/inc \
	$(TARGET_OUT_HEADERS)/common/inc \
	$(commonIncludes) \
	vendor/qcom/opensource/time-services

LOCAL_SHARED_LIBRARIES := \
	libc \
	libcutils \
	libqmi_cci \
	libqmi_common_so

LDLIBS += -lpthread

LOCAL_MODULE:= time_daemon
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

########################## QMI version End ###############################################

endif
