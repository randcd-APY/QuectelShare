#ACLOCAL_AMFLAGS = -I m4

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/sensors
MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2

#libmmcamera2_sensor_modules.so
lib_LTLIBRARIES = libmmcamera2_sensor_modules.la

libmmcamera2_sensor_modules_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera2_sensor_modules_la_CFLAGS += -DMSM_CAMERA_GCC

	ifeq ($(KERNEL_VER), 4.9%)
#libmmcamera2_sensor_modules_la_CFLAGS += -DUSE_4_9_DEFS
	endif

libmmcamera2_sensor_modules_la_CFLAGS += -fPIC
libmmcamera2_sensor_modules_la_CFLAGS += -include stdint.h
libmmcamera2_sensor_modules_la_CFLAGS += -DLOGE=ALOGE
libmmcamera2_sensor_modules_la_CFLAGS += -include utils/Log.h
libmmcamera2_sensor_modules_la_CFLAGS += -D_ANDROID_

#if MSM8909
libmmcamera2_sensor_modules_la_CFLAGS += -DMSM8909_SENSORS
libmmcamera2_sensor_modules_la_CFLAGS += -DAF_2X13_FILTER_SUPPORT
#endif

libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/includes
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/includes

libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/includes
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/controller
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tuning
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug

libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/includes
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/includes
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/module
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/sensors
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix/0301
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuators
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuators/0301
libmmcamera2_sensor_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuator_libs
libmmcamera2_sensor_modules_la_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/

#TODO::
#LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
#LOCAL_C_INCLUDES += \
# $(LOCAL_PATH)/$(MM2_PATH)/$(MM2_PATH)/hardware/qcom/camera/QCamera2/stack/common

libmmcamera2_sensor_modules_la_SOURCES = module/sensor_util.c
libmmcamera2_sensor_modules_la_SOURCES += module/port_sensor.c
libmmcamera2_sensor_modules_la_SOURCES += module/module_sensor.c
libmmcamera2_sensor_modules_la_SOURCES += module/sensor_init.c
libmmcamera2_sensor_modules_la_SOURCES += sensors/sensor.c
libmmcamera2_sensor_modules_la_SOURCES += sensors/sensor_thread.c
libmmcamera2_sensor_modules_la_SOURCES += chromatix/module/chromatix_sub_module.c
libmmcamera2_sensor_modules_la_SOURCES += actuators/actuator.c
libmmcamera2_sensor_modules_la_SOURCES += eeprom/eeprom.c
libmmcamera2_sensor_modules_la_SOURCES += led_flash/led_flash.c
libmmcamera2_sensor_modules_la_SOURCES += strobe_flash/strobe_flash.c
libmmcamera2_sensor_modules_la_SOURCES += csiphy/csiphy.c
libmmcamera2_sensor_modules_la_SOURCES += csid/csid.c

#LOCAL_SHARED_LIBRARIES := libdl liblog libcutils liboemcamera
libmmcamera2_sensor_modules_la_LDFLAGS = -shared
libmmcamera2_sensor_modules_la_LIBADD = -ldl
libmmcamera2_sensor_modules_la_LIBADD += $(BUILD_PATH)/media-controller/mct/liboemcamera.la
dirs =
#dirs += actuator_libs
SUBDIRS = $(dirs)
