ACLOCAL_AMFLAGS = -I m4

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/stats
MM2_PATH = $(top_srcdir)/mm-camera2

#libmmcamera2_stats_modules.so
lib_LTLIBRARIES = libmmcamera2_stats_modules.la

libmmcamera2_stats_modules_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)

	ifeq ($(KERNEL_VER), 4.9%)
#libmmcamera2_stats_modules_la_CFLAGS += -DUSE_4_9_DEFS
	endif

libmmcamera2_stats_modules_la_CFLAGS += -D_ANDROID_
libmmcamera2_stats_modules_la_CFLAGS += -DMSM_CAMERA_GCC
libmmcamera2_stats_modules_la_CFLAGS += -fPIC
libmmcamera2_stats_modules_la_CFLAGS += -Dstrlcpy=g_strlcpy
libmmcamera2_stats_modules_la_CFLAGS += -DFEATURE_GYRO_ANDROID=false
libmmcamera2_stats_modules_la_CFLAGS += -DUSE_GYRO=false
libmmcamera2_stats_modules_la_CFLAGS += -DAF_2X13_FILTER_SUPPORT
libmmcamera2_stats_modules_la_CFLAGS += -D_AWB_EXT_ALGO_FNAME='"dummy.so"'
libmmcamera2_stats_modules_la_CFLAGS += -DFEATURE_GYRO=false
libmmcamera2_stats_modules_la_CFLAGS += -include stdint.h

#TODO:
#if FEATURE_GYRO
#libmmcamera2_stats_modules_la_CFLAGS += -DFEATURE_GYRO
#endif
if FEATURE_SENSOR_HAL
libmmcamera2_stats_modules_la_CFLAGS += -DFEATURE_SENSOR_HAL
endif

#if FEATURE_GYRO_DUMP_DATA
#libmmcamera2_stats_modules_la_CFLAGS += -DGYRO_DUMP_DATA
#endif



libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)

libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/includes/
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/module/
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/includes
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/includes
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/module
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/sensors
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/includes
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix/0301
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuators/0301
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuator_libs
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/actuator_libs

libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/is
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/q3a
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/q3a/aec
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/q3a/af
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/q3a/awb
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/asd
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/afd
#libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/gyro
#libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/gyro/dsps
libmmcamera2_stats_modules_la_CFLAGS += -I $(CURR_PATH)/debug-data

libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/includes
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/controller
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tuning
libmmcamera2_stats_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug
libmmcamera2_stats_modules_la_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/
#sensor path
libmmcamera2_stats_modules_la_CFLAGS += -I $(WORKSPACE)/sensors/sensors/dsps/api
libmmcamera2_stats_modules_la_CFLAGS += -I $(WORKSPACE)/qmi/core/lib/inc
libmmcamera2_stats_modules_la_CXXFLAGS = $(libmmcamera2_stats_modules_la_CFLAGS) $(AM_CPPFLAGS)

libmmcamera2_stats_modules_la_SOURCES = afd/afd_module.c \
                                        afd/afd_thread.c \
                                        afd/afd_port.c \
                                        asd/asd_module.c \
                                        asd/asd_thread.c \
                                        asd/asd_port.c

libmmcamera2_stats_modules_la_SOURCES += q3a/q3a_module.c \
                                        q3a/q3a_thread.c \
                                        q3a/awb/awb_module.c \
                                        q3a/awb/awb_port.c \
                                        q3a/aec/aec_module.c \
                                        q3a/aec/aec_port.c \
                                        q3a/q3a_port.c \
                                        q3a/af/af_port.c \
                                        q3a/af/af_module.c \
                                        q3a/af/af_fdprio.c \
                                        q3a/af/depth_service.c

libmmcamera2_stats_modules_la_SOURCES += is/is_thread.c \
                                        is/dis_interface.c \
                                        is/is_set.c \
                                        is/is_port.c \
                                        is/is_process.c \
                                        is/is_module.c \
                                        is/eis2_interface.c \
                                        is/eis_interface.c

libmmcamera2_stats_modules_la_SOURCES += test.c \
                                        test_stats.c \
                                        stats_port.c \
                                        stats_util.c \
                                        stats_module.c

#LOCAL_SHARED_LIBRARIES := libdl libcutils liboemcamera libmmcamera2_stats_algorithm
libmmcamera2_stats_modules_la_LDFLAGS = -shared -llog
libmmcamera2_stats_modules_la_LIBADD = -ldl -lrt -lglib-2.0 -lcutils -llog -lui -lmmcamera2_stats_algorithm #-landroid -lgui -lmmcamera2_is -loemcamera
libmmcamera2_stats_modules_la_LIBADD += $(abs_top_builddir)/mm-camera2/media-controller/mct/liboemcamera.la
libmmcamera2_stats_modules_la_LIBADD += -lmmcamera2_stats_algorithm
libmmcamera2_stats_modules_la_LIBADD += -lmmcamera2_q3a_core
libmmcamera2_stats_modules_la_LIBADD += -lmmcamera2_is
#libmmcamera2_stats_modules_la_LIBADD += -loemcamera

dirs =
SUBDIRS = $(dirs)

