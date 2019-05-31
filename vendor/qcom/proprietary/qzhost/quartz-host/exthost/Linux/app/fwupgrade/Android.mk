LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CLANG := true
LOCAL_MODULE := qca_mgr_daemon
LOCAL_MODULE_OWNER := qti

ROOT_DIR := $(LOCAL_PATH)/../../../..
#INCLUDE_DIR := $(ROOT_DIR)/include
#LIB_DIR :=$(ROOT_DIR)/lib
#QAPI_DIR :=$(ROOT_DIR)/qapi
DAEMON_MODE := false

#Enable off Thread commissioning (Avahi)
ENABLE_OFF_THREAD_COMMISSIONING := false

QUARTZ_PATH := $(ROOT_DIR)/quartz
SERIALIZER_DIR := $(QUARTZ_PATH)/serializer

LOCAL_C_INCLUDES := $(ROOT_DIR)/include \
	$(ROOT_DIR)/include/qapi \
	$(ROOT_DIR)/include/bsp \
	$(SERIALIZER_DIR)/framework/OSAL/include \
	$(SERIALIZER_DIR)/framework/common \
	$(SERIALIZER_DIR)/framework/config \
	$(SERIALIZER_DIR)/framework/host \
	$(SERIALIZER_DIR)/framework/host/qapi \
	$(SERIALIZER_DIR)/genedit_files/common/include \
	$(SERIALIZER_DIR)/genedit_files/fileID \
	$(SERIALIZER_DIR)/genedit_files/host/include \
	$(SERIALIZER_DIR)/manual/common/include \
	$(SERIALIZER_DIR)/manual/host/include

LOCAL_SRC_FILES := fwupgrade.c

ifeq ($(DAEMON_MODE),true)
DEFINES = -D CONFIG_DAEMON_MODE
endif

LOCAL_CFLAGS :=  -pthread -Wall -g -fno-strict-aliasing -Wno-unused-variable -Wno-pointer-sign -std=c99  $(DEFINES)
LOCAL_WHOLE_STATIC_LIBRARIES := host_serializer

include $(BUILD_EXECUTABLE)
