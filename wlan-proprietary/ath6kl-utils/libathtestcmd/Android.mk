LOCAL_PATH := $(call my-dir)

C_INCLUDES := $(TARGET_OUT_HEADERS)/libtcmd \
			  $(LOCAL_PATH)/include \
			  $(LOCAL_PATH)/libtlvutil \
			  $(LOCAL_PATH)/libtlvutil/common \

include $(CLEAR_VARS)

LOCAL_CLANG := true
LOCAL_MODULE = libtestcmd6174
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_CFLAGS = $(L_CFLAGS)
LOCAL_SRC_FILES = athtestcmdlib.c

LOCAL_C_INCLUDES += $(C_INCLUDES)

LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_WHOLE_STATIC_LIBRARIES := libtcmd libtlvutil

ifneq ($(wildcard system/core/libnl_2),)
# ICS ships with libnl 2.0
LOCAL_SHARED_LIBRARIES += libnl_2
else
LOCAL_SHARED_LIBRARIES += libnl
endif

LOCAL_COPY_HEADERS_TO := libtestcmd6174
LOCAL_COPY_HEADERS := testcmd6174.h \
		  include/testcmd.h \
		  include/art_utf_common.h \

include $(BUILD_STATIC_LIBRARY)

