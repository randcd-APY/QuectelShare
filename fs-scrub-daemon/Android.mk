FS_SCRUB_DAEMON_DIR:= $(call my-dir)
LOCAL_PATH:= $(FS_SCRUB_DAEMON_DIR)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc

LOCAL_SRC_FILES := fs-scrub-daemon.c flash_driver_service_v01.c
LOCAL_CFLAGS += -D_GNU_SOURCE -DFS_SCRUB_ANDROID_BUILD -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -lrt -Wno-format
LOCAL_MODULE := fs-scrub-daemon

LOCAL_SHARED_LIBRARIES := libc libcutils libqmi_cci libqmi_common_so
include $(BUILD_EXECUTABLE)
