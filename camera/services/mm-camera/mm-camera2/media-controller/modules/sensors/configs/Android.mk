LOCAL_PATH := $(call my-dir)

ifeq ($(call is-board-platform-in-list, msmcobalt),true)
include $(CLEAR_VARS)
LOCAL_MODULE:= camera_config.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := msmcobalt_camera.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
else ifeq ($(call is-board-platform-in-list, msm8998),true)
include $(CLEAR_VARS)
LOCAL_MODULE:= camera_config.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := msm8998_camera.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
else ifeq ($(call is-board-platform-in-list, sdm660),true)
include $(CLEAR_VARS)
LOCAL_MODULE:= camera_config.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := sdm660_camera.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
else ifeq ($(call is-board-platform-in-list, msm8996),true)
ifeq ($(DRONE_TARGET),true)
include $(CLEAR_VARS)
LOCAL_MODULE:= camera_config.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := msm8996_camera_drone.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
else
include $(CLEAR_VARS)
LOCAL_MODULE:= camera_config.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := msm8996_camera.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
endif
else ifeq ($(call is-board-platform-in-list, msm8976),true)
include $(CLEAR_VARS)
LOCAL_MODULE:= camera_config.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := msm8976_camera.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
else ifeq ($(call is-board-platform-in-list, msm8952),true)
include $(CLEAR_VARS)
LOCAL_MODULE:= camera_config.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := msm8952_camera.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
else ifeq ($(call is-board-platform-in-list, msm8937),true)
include $(CLEAR_VARS)
LOCAL_MODULE:= camera_config.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := msm8937_camera.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
else ifeq ($(call is-board-platform-in-list, msm8953),true)
include $(CLEAR_VARS)
LOCAL_MODULE:= camera_config.xml
LOCAL_MODULE_CLASS := EXECUTABLES
# Use a custom camera config for boot optimizations
ifeq ($(CAMERA_BOOT_OPTS),true)
  LOCAL_SRC_FILES := msm8953_iot_camera_boot_opts.xml
else
  LOCAL_SRC_FILES := msm8953_camera.xml
endif
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
endif

include $(CLEAR_VARS)
LOCAL_MODULE:= imx214_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx214_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov7251_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov7251_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx274_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx274_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ar1335_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ar1335_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx230_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
ifeq ($(CHROMATIX_VERSION), 0308E)
LOCAL_SRC_FILES := imx230_chromatix_ext.xml
else
LOCAL_SRC_FILES := imx230_chromatix.xml
endif
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx230_qc2002_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx230_qc2002_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx230_qc2002_with_gyro_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx230_qc2002_with_gyro_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= csidtg_camera.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := csidtg_camera.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= csidtg_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := csidtg_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx258_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx258_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx258_bear_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx258_bear_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx258_lc898217xc_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx258_lc898217xc_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov4688_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov4688_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov5670_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov5670_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov5670_f5670bq_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov5670_f5670bq_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov8856_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov8856_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov8858_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov8858_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov8865_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov8865_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov13850_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov13850_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k2l7_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k2l7_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov13850_q13v06k_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov13850_q13v06k_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k3m2xm_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k3m2xm_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k3m2xm_chromatix_bear.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k3m2xm_chromatix_bear.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k3m2xx_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k3m2xx_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov5695_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov5695_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx298_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx298_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx298_gt24c64_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx298_gt24c64_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k3l8_f3l8yam_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k3l8_f3l8yam_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k3l8_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k3l8_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k3l8_mono_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k3l8_mono_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx362_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx362_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx362_chromatix_bear.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx362_chromatix_bear.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx362_gt24c64a_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx362_gt24c64a_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx318_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx318_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k2l7_8976_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k2l7_8976_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx258_mono_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx258_mono_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx258_mono_bear_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx258_mono_bear_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx378_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx378_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx477_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
ifeq ($(ENABLED_IMX477_RAW10_MODES),true)
LOCAL_SRC_FILES := imx477_raw10_chromatix.xml
else
LOCAL_SRC_FILES := imx477_raw12_chromatix.xml
endif
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov4685_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov4685_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k2l7_8953_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k2l7_8953_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov2680_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov2680_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= ov2281_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := ov2281_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k3p3sm_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k3p3sm_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k2l7sx_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k2l7sx_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k3m3sm_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k3m3sm_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= s5k3p8sp_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := s5k3p8sp_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE:= imx377_chromatix.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := imx377_chromatix.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/camera
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)
