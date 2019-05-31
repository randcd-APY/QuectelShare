LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=     \
    src/btsnoop_dump.c    \
    src/bt_logger.c   \
    src/bt_log_buffer.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/incl

LOCAL_MODULE:= bt_logger

LOCAL_SHARED_LIBRARIES += libcutils

include $(BUILD_EXECUTABLE)
