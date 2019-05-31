ACLOCAL_AMFLAGS = -I m4

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/mct
MM2_PATH = $(top_srcdir)/mm-camera2

#liboemcamera.so
lib_LTLIBRARIES = liboemcamera.la

liboemcamera_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
liboemcamera_la_CFLAGS += -DMSM_CAMERA_GCC
liboemcamera_la_CFLAGS += -fPIC
liboemcamera_la_CFLAGS += -include stdint.h
liboemcamera_la_CFLAGS += -DLOGE=ALOGE
liboemcamera_la_CFLAGS += -include utils/Log.h

liboemcamera_la_CFLAGS += -DUSE_ION
if MSM8909
liboemcamera_la_CFLAGS += -DCAMERA_ION_HEAP_ID=ION_CP_MM_HEAP_ID
else
liboemcamera_la_CFLAGS += -DCAMERA_ION_HEAP_ID=ION_CAMERA_HEAP_ID
endif

#if DRONE_TARGET
#liboemcamera_la_CFLAGS += -D_DRONE_
#endif

#if VFE40
#liboemcamera_la_CFLAGS += -DVFE_40
#endif
liboemcamera_la_CFLAGS += -DIPL_DEBUG_STANDALONE

liboemcamera_la_CFLAGS += -I $(CURR_PATH)/../../includes/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/../../../common/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/../includes/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/tools/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/bus/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/controller/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/event/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/module/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/object/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/pipeline/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/port/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/stream/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/tuning/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/debug/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/../../server-imaging/
liboemcamera_la_CFLAGS += -I $(CURR_PATH)/../modules/includes/
liboemcamera_la_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/
liboemcamera_la_CFLAGS += -I $(MM2_PATH)/server-tuning/tuning

	ifeq ($(KERNEL_VER), 4.9%)
#liboemcamera_la_CFLAGS += -DUSE_4_9_DEFS
	endif

# all files under mct compiled into liboemcamera.so
liboemcamera_la_SOURCES = bus/mct_bus.c
liboemcamera_la_SOURCES += tuning/mct_tuning.c
liboemcamera_la_SOURCES += tools/mct_list.c
liboemcamera_la_SOURCES += tools/mct_queue.c
liboemcamera_la_SOURCES += pipeline/mct_pipeline.c
liboemcamera_la_SOURCES += controller/mct_controller.c
liboemcamera_la_SOURCES += stream/mct_stream.c
liboemcamera_la_SOURCES += port/mct_port.c
liboemcamera_la_SOURCES += object/mct_object.c
liboemcamera_la_SOURCES += module/mct_module.c
liboemcamera_la_SOURCES += debug/server_debug.c

#LOCAL_SHARED_LIBRARIES:= libdl libcutils  liblog

liboemcamera_la_LDFLAGS = -shared -llog
liboemcamera_la_LIBADD = -ldl -lrt
