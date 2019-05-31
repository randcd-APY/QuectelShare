LOCAL_PATH:= $(call my-dir)

###############################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := dsrc_tx.c src/dsrc_util.c src/dsrc_sock.c src/crc.c getsubopt.c
LOCAL_MODULE := dsrc_tx
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc $(LOCAL_PATH)/src
LOCAL_CFLAGS += -D_XOPEN_SOURCE=500 -D_ANDROID_

include $(BUILD_EXECUTABLE)

###############################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := dsrc_rx.c src/dsrc_util.c src/dsrc_sock.c src/crc.c getsubopt.c
LOCAL_MODULE := dsrc_rx
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc $(LOCAL_PATH)/src
LOCAL_CFLAGS += -D_XOPEN_SOURCE=500 -D_ANDROID_

include $(BUILD_EXECUTABLE)

###############################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := dsrc_set_chan.c src/dsrc_util.c getsubopt.c
LOCAL_MODULE := dsrc_set_chan
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc $(LOCAL_PATH)/src
LOCAL_CFLAGS += -D_XOPEN_SOURCE=500 -D_ANDROID_

include $(BUILD_EXECUTABLE)

###############################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := dsrc_config.c src/dsrc_nl.c dsrc_dcc.c getsubopt.c
LOCAL_MODULE := dsrc_config
LOCAL_SHARED_LIBRARIES += libnl
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc $(LOCAL_PATH)/src
LOCAL_CFLAGS += -D_XOPEN_SOURCE=500 -D_ANDROID_

include $(BUILD_EXECUTABLE)

###############################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := dcc.dat
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := dcc.dat
include $(BUILD_PREBUILT)
