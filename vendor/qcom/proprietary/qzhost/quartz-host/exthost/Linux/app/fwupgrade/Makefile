ifeq ($(DK_CROSS_COMPILE),true)
	CC = arm-openwrt-linux-muslgnueabi-gcc
else
	CC := gcc
endif

ROOT_DIR := ../../../..
LINUX_PATH := ../..
APP_DIR := ..
INCLUDE_DIR := $(ROOT_DIR)/include
LIB_DIR :=$(ROOT_DIR)/lib
QAPI_DIR :=$(ROOT_DIR)/qapi
DAEMON_MODE := true

#Enable off Thread commissioning (Avahi)
ENABLE_OFF_THREAD_COMMISSIONING := false

QUARTZ_PATH := $(ROOT_DIR)/quartz
SERIALIZER_DIR := $(QUARTZ_PATH)/serializer
SERIALIZER_LIB_DIR := $(LINUX_PATH)/qapi/output

INCLUDE_DIRS := -I. \
	-I$(ROOT_DIR)/include \
	-I$(ROOT_DIR)/include/qapi \
	-I$(ROOT_DIR)/include/bsp \
	-I$(SERIALIZER_DIR)/framework/OSAL/include \
	-I$(SERIALIZER_DIR)/framework/common \
	-I$(SERIALIZER_DIR)/framework/config \
	-I$(SERIALIZER_DIR)/framework/host \
	-I$(SERIALIZER_DIR)/framework/host/qapi \
	-I$(SERIALIZER_DIR)/genedit_files/common/include \
	-I$(SERIALIZER_DIR)/genedit_files/fileID \
	-I$(SERIALIZER_DIR)/genedit_files/host/include \
	-I$(SERIALIZER_DIR)/manual/common/include \
	-I$(SERIALIZER_DIR)/manual/host/include

SRCS := $(wildcard $(APP_DIR)/fwupgrade/*.c)
OBJS := $(SRCS:%.c=%.o)

ifeq ($(DAEMON_MODE),true)
DEFINES = -D CONFIG_DAEMON_MODE
endif

CFLAGS := $(INCLUDE_DIRS) -pthread -Wall -g -fno-strict-aliasing -Wno-unused-variable -Wno-pointer-sign -std=c99  $(DEFINES)

#ifneq ($(DK_CROSS_COMPILE), true)
#CFLAGS += -m32
#endif

LDFLAGS = -lpthread -lrt 

.PHONY:
all: qca_mgr_daemon

.PHONY:
qca_mgr_daemon: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(SERIALIZER_LIB_DIR)/host_serializer.a -o qca_mgr_daemon $(LDFLAGS)

.PHONY:
clean:
	rm -rf $(OBJS)
	rm -f qca_mgr_daemon
