ROOT_DIR := ../../../../..
LINUX_PATH := ../../..
APP_DIR := ../..
DEMO_DIR := ..
INCLUDE_DIR := $(ROOT_DIR)/include
LIB_DIR :=$(ROOT_DIR)/lib
QAPI_DIR :=$(ROOT_DIR)/qapi
DAEMON_MODE := true

#Enable off Thread commissioning (Avahi)
ENABLE_OFF_THREAD_COMMISSIONING := false

QUARTZ_PATH := $(ROOT_DIR)/quartz
SERIALIZER_DIR := $(QUARTZ_PATH)/serializer
SERIALIZER_LIB_DIR := $(LINUX_PATH)/qapi/output
#QURT_LIB_DIR := $(APP_DIR)/qurt/output

INCLUDE_DIRS := -I. \
	-I$(ROOT_DIR)/include \
	-I$(ROOT_DIR)/include/qapi \
	-I$(ROOT_DIR)/include/bsp \
	-I$(QUARTZ_PATH)/bluetopia/overlay/btpsver \
	-I$(SERIALIZER_DIR)/framework/OSAL/include \
	-I$(SERIALIZER_DIR)/framework/common \
	-I$(SERIALIZER_DIR)/framework/config \
	-I$(SERIALIZER_DIR)/framework/host \
	-I$(SERIALIZER_DIR)/framework/host/qapi \
	-I$(SERIALIZER_DIR)/genedit_files/common/include \
	-I$(SERIALIZER_DIR)/genedit_files/fileID \
	-I$(SERIALIZER_DIR)/genedit_files/host/include \
	-I$(SERIALIZER_DIR)/manual/common/include \
	-I$(SERIALIZER_DIR)/manual/host/include \
	-I$(SERIALIZER_DIR)/herh/inc \
	-I$(DEMO_DIR)/src/spple \
	-I$(DEMO_DIR)/src/debug \
	-I$(DEMO_DIR)/src/zigbee \
	-I$(DEMO_DIR)/src/zigbee/clusters \
	-I$(DEMO_DIR)/src/hmi \
	-I$(DEMO_DIR)/src/thread \
	-I$(DEMO_DIR)/src/fwupgrade \
	-I$(DEMO_DIR)/src/coex \
	-I$(DEMO_DIR)/src/qcli

SRCS := $(wildcard $(DEMO_DIR)/src/qcli/*.c) \
	$(wildcard $(DEMO_DIR)/src/spple/*.c) \
	$(wildcard $(DEMO_DIR)/src/debug/*.c) \
	$(wildcard $(DEMO_DIR)/src/hmi/*.c) \
	$(wildcard $(DEMO_DIR)/src/thread/*.c) \
	$(wildcard $(DEMO_DIR)/src/zigbee/*.c) \
	$(wildcard $(DEMO_DIR)/src/zigbee/clusters/*.c) \
	$(wildcard $(DEMO_DIR)/src/fwupgrade/*.c) \
	$(wildcard $(DEMO_DIR)/src/coex/*.c)

OBJS := $(SRCS:%.c=%.o)

CFLAGS := $(INCLUDE_DIRS) -pthread -Wall -g -fno-strict-aliasing -Wno-unused-variable -Wno-pointer-sign -std=gnu99

ifeq ($(DAEMON_MODE),true)
 DEFINES := -DCONFIG_DAEMON_MODE
endif

ifdef HOST_CROSS_COMPILE
  ifeq ($(HOST_CROSS_COMPILE),dakota)
    CC := arm-openwrt-linux-muslgnueabi-gcc
    export STAGING_DIR := $(CURDIR)
  endif
  ifeq ($(HOST_CROSS_COMPILE),ipq807x)
    CC := arm-openwrt-linux-muslgnueabi-gcc
    export STAGING_DIR := $(CURDIR)
  endif

else
  CFLAGS += -m32
  CC := gcc
endif

CFLAGS += $(DEFINES)

LDFLAGS = -lpthread -lrt 

.PHONY:
all: nb_demo

.PHONY:
nb_demo: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(SERIALIZER_LIB_DIR)/host_serializer.a -o nb_demo $(LDFLAGS)

.PHONY:
clean:
	rm -rf $(OBJS)
	rm -f nb_demo
