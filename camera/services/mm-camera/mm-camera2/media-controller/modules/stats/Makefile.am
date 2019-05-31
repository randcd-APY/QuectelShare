#======================================================================
#makefile for libmmcamera2_stats_modules.so form mm-camera2
#======================================================================
#======================================================================
#======================================================================
ACLOCAL_AMFLAGS = -I m4
LOCAL_PATH = $(top_srcdir)/media-controller/modules/stats
QC_PROP_ROOT = $(WORKSPACE)/camera/services
LOCAL_MMCAMERA_PATH = $(QC_PROP_ROOT)/mm-camera/mm-camera2

LOCAL_CFLAGS  = -Werror

LOCAL_CFLAGS += -DDSPS_DISABLE_WAIT_FOR_CALLBACK

if IS_CHROMATIX_VERSION_0308E
LOCAL_CFLAGS += -DCHROMATIX_308E
endif

if !USE_GYRO
LOCAL_CFLAGS += -DDISABLE_GYRO_MODULE
endif

if FEATURE_GYRO_IMU
LOCAL_CFLAGS += -DFEATURE_GYRO_IMU
endif

if FEATURE_GYRO_DSPS
LOCAL_CFLAGS += -DFEATURE_GYRO_DSPS
endif

if FEATURE_DG
LOCAL_CFLAGS += -DFEATURE_DG
endif

if !IS_ENABLE
LOCAL_CFLAGS += -DDISABLE_IS_MODULE
endif


LOCAL_C_INCLUDES = -I$(LOCAL_PATH)
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/chromatix-wrapper/
LOCAL_C_INCLUDES += -I$(LOCAL_PATH)/debug-data
LOCAL_C_INCLUDES += -I$(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)/
LOCAL_C_INCLUDES += -I$(MM_CAMERASDK_PATH)/sensor/includes
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/module/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/includes/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/include
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/platform/linux
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/aec
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/awb
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/test
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/asd
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/afd
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/af
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/af/af_algo_interface
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/q3a/af/examples

LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/is
LOCAL_C_INCLUDES += -I$(QC_PROP_ROOT)/mm-camera-noship/is/sensor_lib

if USE_GYRO
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/gyro
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/gyro/dsps
LOCAL_C_INCLUDES += -I$(WORKSPACE)/sensors/sensors/dsps/api
endif

LOCAL_C_INCLUDES += -I$(QC_PROP_ROOT)/qmi/core/lib/inc
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/includes/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/controller/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/event/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/module/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/object/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/port/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/fdleak
LOCAL_C_INCLUDES += -I$(SRC_XML_INCLUDE)
LOCAL_C_INCLUDES += -I$(LOCAL_PATH)/external/libxml2/include
LOCAL_C_INCLUDES += -I$(LOCAL_PATH)/external/icu/icu4c/source/common

#add gyro sensor API header locations
LOCAL_C_INCLUDES += -I$(COMMON_INCDIR)/sensors
LOCAL_C_INCLUDES += -I$(COMMON_INCDIR)/qmi
LOCAL_C_INCLUDES += -I$(COMMON_INCDIR)/adreno
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/is/EIS_DG
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/is/Digital_Gimbal
LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/gyro/imu

if FEATURE_GYRO_IMU
LOCAL_C_INCLUDES += -I$(WORKSPACE)/FEAT-API-LNX-IMU/apps_proc/inc
endif
#For DG, which is copied from drones-prebuilts
LOCAL_C_INCLUDES += -I$(WORKSPACE)/FEAT-API-LNX-DG/apps_proc
#For EIS_DG
LOCAL_C_INCLUDES += -I$(QC_PROP_ROOT)/mm-camera-noship/is

LOCAL_C_INCLUDES += -I$(COMMON_INCDIR)/eis_dg

LOCAL_C_INCLUDES += -I$(WORKSPACE)/camera/lib/QCamera2/stack/common

LOCAL_CFLAGS  += -Werror -Wunused-parameter

LOCAL_SRC_FILES = \
        external/ext_awb_wrapper.c \
        afd/afd_port.c \
        afd/afd_thread.c \
        afd/afd_module.c \
        stats_port.c \
        stats_util.c \
        test_stats.c \
        asd/asd_module.c \
        asd/asd_port.c \
        asd/asd_thread.c \
        asd/asd_port_ext.c \
        asd/asd_ext_algo_wrapper.c \
        test.c \
        q3a/aec/aec_port.c \
        q3a/aec/aec_module.c \
        q3a/aec/aec_port_ext.c \
        q3a/aec/aec_biz.c \
        q3a/q3a_module.c \
        q3a/q3a_thread_ext.c \
        q3a/q3a_thread.c \
        q3a/platform/linux/q3a_platform.c \
        q3a/test/af_ext_test_wrapper.c \
        q3a/test/awb_ext_test_wrapper.c \
        q3a/test/aec_ext_test_wrapper.c \
        q3a/q3a_port.c \
        q3a/awb/awb_port.c \
        q3a/awb/awb_port_ext.c \
        q3a/awb/awb_module.c \
        q3a/awb/awb_biz.c \
        stats_module.c \
        chromatix-wrapper/stats_chromatix_wrapper.c

LOCAL_SRC_FILES += \
	q3a/af/af_port.c \
	q3a/af/af_algo_params.c \
	q3a/af/examples/af_haf_TOF_decorator.c \
	q3a/af/examples/af_haf_custom_mixer.c \
	q3a/af/examples/af_haf_example.c \
	q3a/af/af_module.c \
	q3a/af/af_biz.c \
	q3a/af/af_algo_interface/af_alg_factory_custom.c \
	q3a/af/af_port_ext.c \
	q3a/af/af_fdprio.c \
	q3a/af/depth_service.c 

if USE_GYRO
LOCAL_SRC_FILES += \
	gyro/gyro_port.c \
	gyro/gyro_module.c \
	gyro/dsps/dsps_hw_interface.c \
	gyro/dsps/sensor_intf.c \
	gyro/dsps/sensor_laser.c
if FEATURE_GYRO_DSPS
LOCAL_SRC_FILES += \
	gyro/dsps/sensor_service_util.cpp \
	gyro/dsps/sensor_native.c \
	gyro/dsps/dsps_hw.c
endif
endif

if FEATURE_GYRO_IMU
LOCAL_SRC_FILES += \
	gyro/imu/imu_driver.c
endif

if IS_ENABLE
LOCAL_SRC_FILES += \
	is/is_port.c \
	is/is_set.c \
	is/EIS_DG/eis_dg_interface.c \
	is/EIS_DG/mesh_fusion_interface.c \
	is/is_module.c \
	is/eis2_interface.c \
	is/is_thread.c \
	is/dis_interface.c \
	is/is_process.c
endif

if FEATURE_DG
LOCAL_SRC_FILES += \
    is/Digital_Gimbal/digital_gimbal_interface.c
endif

LOCAL_C_INCLUDES += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/stats/external

# Add Enable extension, and specify prebuilt so name
LOCAL_CFLAGS += -D_AWB_EXT_ALGO_FNAME='"dummy.so"'

lib_LTLIBRARIES = libmmcamera2_stats_modules.la

libmmcamera2_stats_modules_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera2_stats_modules_la_LDFLAGS = -shared
libmmcamera2_stats_modules_la_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
libmmcamera2_stats_modules_la_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap
libmmcamera2_stats_modules_la_CPPFLAGS =  $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) $(AM_CPPFLAGS)
libmmcamera2_stats_modules_la_LIBADD = ../../mct/libmmcamera2_mct.la

#Shared libraries
libmmcamera2_stats_modules_la_LIBADD += -ldl -lcutils -lmmcamera2_stats_lib -lmmcamera2_stats_algorithm -lui -lxml2 
libmmcamera2_stats_modules_la_LIBADD += -lutils -lmmcamera_dbg

if IS_ENABLE
libmmcamera2_stats_modules_la_LIBADD += -lmmcamera2_is
endif

if FEATURE_GYRO_DSPS
libmmcamera2_stats_modules_la_LIBADD += -lsensor1
endif
