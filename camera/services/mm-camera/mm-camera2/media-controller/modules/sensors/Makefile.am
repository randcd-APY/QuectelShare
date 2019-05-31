LOCAL_PATH = $(top_srcdir)/media-controller/modules/sensors
MMCAMERA_PATH  = $(LOCAL_PATH)/../../../../mm-camera2
LOCAL_SENSOR_PATH = $(MMCAMERA_PATH)/media-controller/modules/sensors/

LOCAL_CFLAGS = -Werror

LOCAL_CPPFLAGS = -I$(LOCAL_PATH)
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/includes/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/includes/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/bus/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/controller/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/event/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/module/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/object/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/pipeline/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/port/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/stream/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/tools/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/debug/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/mct/debug/fdleak
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/modules/includes/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/server-tuning/tuning/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/modules/sensors/module/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/modules/sensors/sensor/module/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/modules/sensors/sensor/libs/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/modules/sensors/eebin/module/include/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/modules/sensors/eebin/module/
LOCAL_CPPFLAGS += -I$(MM_CAMERASDK_PATH)/sensor/includes/
LOCAL_CPPFLAGS += -I$(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/modules/sensors/ois/module/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/modules/sensors/ois/libs/
LOCAL_CPPFLAGS += -I$(MMCAMERA_PATH)/media-controller/modules/sensors/live_connect/
LOCAL_CPPFLAGS += -I external/zlib
LOCAL_CPPFLAGS += -I external/libxml2/include
LOCAL_CPPFLAGS += -I$(SRC_XML_INCLUDE)
LOCAL_CPPFLAGS += -I external/icu/icu4c/source/common
LOCAL_CPPFLAGS += -I$(CAMERAHAL_PATH)/QCamera2/stack/common

if ENABLE_msm8996
LOCAL_CPPFLAGS  += -DMSM8996_SENSORS
endif

if ENABLE_msm8998
LOCAL_CPPFLAGS  += -DMSM8998_SENSORS
endif


if ENABLE_msm8953
LOCAL_CPPFLAGS  += -DTITANIUM_SENSORS
endif


if ENABLED_IMX477_RAW10_MODES
LOCAL_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
else
#disable IMX477 RAW10 mode and enable RAW12.
endif

c_source = \
        module/sensor_util.c \
        module/port_sensor.c \
        module/module_sensor_offload.c \
        module/sensor_init.c \
        module/sensor_frame_control.c \
        module/sensor_fast_aec.c \
        module/sensor_util_xml.c \
        module/module_sensor.c \
        module/sensor_sdk_utils.c \
        sensor/module/sensor.c \
        sensor/module/sensor_pdaf_api.c \
        sensor/module/PdafMathFunc.c \
        sensor/module/sensor_thread.c \
        sensor/module/sensor_pick_res.c \
        sensor/module/sensor_delay_api.c \
        sensor/module/sensor_gain_exposure.c \
        sensor/module/PdafLibrary.c \
        chromatix/module/chromatix_sub_module.c \
        chromatix/module/chromatix_manager.c \
        chromatix/module/chromatix_hash.c \
        chromatix/module/chromatix_list.c \
        chromatix/module/chromatix_lru.c \
        actuator/module/actuator.c \
        ois/module/ois.c \
        eeprom/libs/sunny_gt24c64a_imx362/sunny_gt24c64a_imx362_eeprom.c \
        eeprom/libs/qtech_f3l8yam/qtech_f3l8yam_eeprom.c \
        eeprom/libs/sunny_gt24c64_s5k2l7sx/sunny_gt24c64_s5k2l7sx_eeprom.c \
        eeprom/libs/sony_imx378/sony_imx378_eeprom.c \
        eeprom/libs/sunny_8865/sunny_8865_eeprom.c \
        eeprom/libs/imx258_mono_gt24c32/imx258_mono_gt24c32_eeprom.c \
        eeprom/libs/truly_cmb433/truly_cmb433_eeprom.c \
        eeprom/libs/dw9761b_2d/dw9761b_2d_eeprom.c \
        eeprom/libs/le2464c_master/le2464c_master_eeprom.c \
        eeprom/libs/sony_imx135/sony_imx135_eeprom.c \
        eeprom/libs/sony_imx298/sony_imx298_eeprom.c \
        eeprom/libs/sony_imx214/sony_imx214_eeprom.c \
        eeprom/libs/sunny_q13v06k/sunny_q13v06k_eeprom.c \
        eeprom/libs/m24c64s/m24c64s_eeprom.c \
        eeprom/libs/ov4688/ov4688_eeprom.c \
        eeprom/libs/imx258_gt24c16/imx258_gt24c16_eeprom.c \
        eeprom/libs/onsemi_cat24c16/onsemi_cat24c16_eeprom.c \
        eeprom/libs/truly_cma481/truly_cma481_eeprom.c \
        eeprom/libs/eeprom_util/eeprom_util.c \
        eeprom/libs/atmel_at24c32e/atmel_at24c32e.c \
        eeprom/libs/rohm_brcg064gwz_3/rohm_brcg064gwz_3_eeprom.c \
        eeprom/libs/sunny_q5v41b/sunny_q5v41b_eeprom.c \
        eeprom/libs/qtech_f5670bq/qtech_f5670bq_eeprom.c \
        eeprom/libs/le2464c/le2464c_eeprom.c \
        eeprom/libs/sunny_gt24c64_s5k2l7/sunny_gt24c64_s5k2l7_eeprom.c \
        eeprom/libs/rohm_brce064gwz/rohm_brce064gwz.c \
        eeprom/libs/sunny_gt24c64_imx298/sunny_gt24c64_imx298_eeprom.c \
        eeprom/libs/imx258_gt24c32/imx258_gt24c32_eeprom.c \
        eeprom/libs/onsemi_cat24c32/onsemi_cat24c32.c \
        eeprom/libs/onsemi_cat24c32_imx362/onsemi_cat24c32_imx362.c \
        eeprom/module/eeprom_dump.c \
        eeprom/module/eeprom.c \
        flash/module/flash.c \
        csiphy/csiphy.c \
        csid/csid.c \
        eebin/libs/eebin_parse.c \
        eebin/module/eebin_interface.c \
        eebin/module/eebin.c \
        eebin/module/eebin_compress.c \
        external/external.c \
        live_connect/live_connect_items.c \
        live_connect/live_connect.c \
        ir_led/ir_led.c \
        ir_cut/ir_cut.c

LOCAL_LDFLAGS  = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

LOCAL_CPPFLAGS += -DMOD_SENSOR_LIB_PATH=\"/usr/lib\"

lib_LTLIBRARIES = libmmcamera2_sensor_modules.la

libmmcamera2_sensor_modules_la_CPPFLAGS = $(LOCAL_CPPFLAGS)
libmmcamera2_sensor_modules_la_SOURCES = $(c_source)
libmmcamera2_sensor_modules_la_LIBADD = -lcutils -llog ../../mct/libmmcamera2_mct.la -lz -lxml2 -lmmcamera_dbg \
					-lmmcamera_pdaf -lmmcamera_pdafcamif
libmmcamera2_sensor_modules_la_LDFLAGS = -shared $(LOCAL_LDFLAGS)

libmmcamera2_sensor_modulesdir = $(rootdir)/system/etc/camera
dist_libmmcamera2_sensor_modules_DATA  = \ 
    configs/imx230_qc2002_chromatix.xml \
    configs/imx274_chromatix.xml \
    configs/imx230_qc2002_with_gyro_chromatix.xml \
    configs/imx230_chromatix.xml \
    configs/imx477_raw10_chromatix.xml \
    configs/imx378_chromatix.xml \
    configs/imx477_raw12_chromatix.xml \
    configs/ar1335_chromatix.xml \
    configs/imx377_chromatix.xml \
    configs/ov7251_chromatix.xml \
    configs/ov9281_chromatix.xml

dirs = 
dirs += sensor/libs
dirs += eeprom/libs
dirs += actuator/libs
dirs += eebin/libs
dirs += ois/libs
dirs += flash/libs
if IS_CHROMATIX_VERSION_0309
dirs += chromatix/0309
endif

if IS_CHROMATIX_VERSION_0310
dirs += chromatix/0310
endif
SUBDIRS = $(dirs)
