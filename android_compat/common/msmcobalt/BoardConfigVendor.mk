## BoardConfigVendor.mk
## Qualcomm Technologies proprietary product specific compile-time definitions.
#
ifeq ($(BOARD_PRESIL_BUILD), true)
    USE_CAMERA_STUB := true
    DEVICE_PACKAGE_OVERLAYS += vendor/qcom/proprietary/common/msmcobalt/overlay
    BOARD_USES_DPM := true
    BOARD_USES_QCNE := true
    TARGET_USES_IMS := true_
    TARGET_SCVE_DISABLED := true
    TARGET_FASTCV_DISABLED := true
    TARGET_ENABLE_QC_AV_ENHANCEMENTS := true
    USESECIMAGETOOL := true
    ## Gensecimage generation of signed apps bootloader
    QTI_GENSECIMAGE_MSM_IDS := msmcobalt
    ## Use signed image as default
    QTI_GENSECIMAGE_SIGNED_DEFAULT := msmcobalt
    #
    ##SECIMAGE tool feature flags
    USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN := 1
    USES_SEC_POLICY_INTEGRITY_CHECK := 1
    USE_SOC_HW_VERSION := true
    SOC_HW_VERSION := 0x30020000
    SOC_VERS := 0x3005
    OVERRIDE_RS_DRIVER := libRSDriver_adreno.so
    BOARD_USES_ADRENO := true
    HAVE_ADRENO_SOURCE := true
    HAVE_ADRENO_SC_SOURCE := true
    HAVE_ADRENO_FIRMWARE := true
    TARGET_USES_ION := true
    ## wlan related flags
    BOARD_HAS_QCOM_WLAN := true
    TARGET_USES_ICNSS_QMI := true
    BOARD_HAS_ATH_WLAN_AR6320 := true
    BOARD_WLAN_DEVICE := qcwcn
    BOARD_WPA_SUPPLICANT_DRIVER := NL80211
    BOARD_HOSTAPD_DRIVER := NL80211
    WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/wlan.ko"
    WIFI_DRIVER_MODULE_NAME := "wlan"
    WIFI_DRIVER_MODULE_ARG := ""
    WIFI_DRIVER_BUILT := qca_cld3
    WIFI_DRIVER_DEFAULT := qca_cld3
    WIFI1_DRIVER_MODULE_ARG := ""
    WPA_SUPPLICANT_VERSION := VER_0_8_X
    HOSTAPD_VERSION := VER_0_8_X
    CONFIG_ACS := true
    CONFIG_IEEE80211AC := true
    WIFI_DRIVER_FW_PATH_STA := "sta"
    WIFI_DRIVER_FW_PATH_AP  := "ap"
    WIFI_DRIVER_FW_PATH_P2P := "p2p"
    BOARD_HAS_CFG80211_KERNEL3_4 := true
    BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
    BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
    BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := default
    BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET := false
    ifneq ($(BUILD_TINY_ANDROID), true)
      BOARD_HAVE_BLUETOOTH := true
      BOARD_HAVE_QCOM_FM := true
      BOARD_ANT_WIRELESS_DEVICE := "qualcomm-uart"
      BOARD_HAS_QCA_BT_SOC := "cherokee"
      ## Add FM chip type
      BOARD_HAS_QCA_FM_SOC := "cherokee"
      QCOM_BT_USE_SIBS := false
      WCNSS_FILTER_USES_SIBS := true

      ifeq ($(BOARD_HAVE_BLUETOOTH), true)
        #JB_MR2 use only bluedroid instead of bluez
        BOARD_HAVE_BLUETOOTH_BLUEZ := false
        ifneq ($(BOARD_HAVE_BLUETOOTH_BLUEZ), true)
          BOARD_HAVE_BLUETOOTH_QCOM := true
          QCOM_BT_USE_SMD_TTY := true
          BOARD_USES_WIPOWER := false
        endif # BOARD_HAVE_BLUETOOTH_BLUEZ
      endif # BOARD_HAVE_BLUETOOTH
    endif # BUILD_TINY_ANDROID
else
    USE_CAMERA_STUB := false
    #
    BOARD_USES_QCOM_HARDWARE := true
    BOARD_USES_ADRENO := true
    HAVE_ADRENO_SOURCE := true
    HAVE_ADRENO_SC_SOURCE := true
    HAVE_ADRENO_FIRMWARE := true
    #DYNAMIC_SHARED_LIBV8SO := true
    ##ENABLE_WEBGL := true
    ##BOARD_USE_QCOM_LLVM_CLANG_RS := true
    TARGET_USES_ION := true
    USE_OPENGL_RENDERER := true
    TARGET_USES_C2D_COMPOSITION := true
    BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := default
    BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET := false
    BOARD_USES_QCNE := true
    BOARD_USES_DPM := true
    #TARGET_USES_ASHMEM := true
    #TARGET_USES_SECURITY_BRIDGE := true
    #TARGET_USES_SEAL_2_0 := true
    #TARGET_USES_POST_PROCESSING := true
    #TARGET_USE_SBC_DECODER := true
    #TARGET_USES_SCALAR := true
    #
    #AUDIO_FEATURE_FLAGS
    #-include $(TOPDIR)vendor/qcom/proprietary/mm-audio-internal/dolby/dax/device/dax2_msmcobalt_hw/dax2_msmcobalt_hw.mk
    ##AUDIO_FEATURE_FLAGS
    #

    USESECIMAGETOOL := true
    ## Gensecimage generation of signed apps bootloader
    QTI_GENSECIMAGE_MSM_IDS := msmcobalt
    ## Use signed image as default
    QTI_GENSECIMAGE_SIGNED_DEFAULT := msmcobalt
    #
    ##SECIMAGE tool feature flags
    USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN := 1
    USES_SEC_POLICY_INTEGRITY_CHECK := 1
    USE_SOC_HW_VERSION := true
    SOC_HW_VERSION := 0x30020000
    SOC_VERS := 0x3005
    ##ENERGY_AWARENESS := true
    #
    ## Enables Adreno RS driver
    OVERRIDE_RS_DRIVER := libRSDriver_adreno.so
    #
    ifneq ($(BUILD_TINY_ANDROID), true)
      BOARD_HAS_QCOM_WLAN := true
      BOARD_HAS_QCOM_WIGIG := true
      #BOARD_HAS_ATH_WLAN_AR6004 := true
      BOARD_HAS_ATH_WLAN_AR6320 := true
      BOARD_HAVE_BLUETOOTH := true
      #BOARD_HAS_QCA_BT_AR3002 := true
      BOARD_HAVE_QCOM_FM := true
      BOARD_ANT_WIRELESS_DEVICE := "qualcomm-uart"
      BOARD_HAS_QCA_BT_SOC := "cherokee"
      ## Add FM chip type
      BOARD_HAS_QCA_FM_SOC := "cherokee"
      QCOM_BT_USE_SIBS := false
      WCNSS_FILTER_USES_SIBS := true

      ifeq ($(BOARD_HAVE_BLUETOOTH), true)
        #JB_MR2 use only bluedroid instead of bluez
        BOARD_HAVE_BLUETOOTH_BLUEZ := false
        ifneq ($(BOARD_HAVE_BLUETOOTH_BLUEZ), true)
          BOARD_HAVE_BLUETOOTH_QCOM := true
          QCOM_BT_USE_SMD_TTY := true
          BOARD_USES_WIPOWER := false
        endif # BOARD_HAVE_BLUETOOTH_BLUEZ
      endif # BOARD_HAVE_BLUETOOTH

      ifeq ($(findstring true,$(BOARD_HAS_ATH_WLAN_AR6004) $(BOARD_HAS_QCOM_WLAN)),true)
        BOARD_WLAN_DEVICE := qcwcn
        BOARD_WPA_SUPPLICANT_DRIVER := NL80211
        BOARD_HOSTAPD_DRIVER := NL80211
        WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/wlan.ko"
        WIFI_DRIVER_MODULE_NAME := "wlan"
        WIFI_DRIVER_MODULE_ARG := ""
        WIFI_DRIVER_BUILT := qca_cld3
        WIFI_DRIVER_DEFAULT := qca_cld3
        WIFI1_DRIVER_MODULE_PATH := "/system/lib/modules/wil6210.ko"
        WIFI1_DRIVER_MODULE_NAME := "wil6210"
        WIFI1_DRIVER_MODULE_ARG := ""
        WPA_SUPPLICANT_VERSION := VER_0_8_X
        HOSTAPD_VERSION := VER_0_8_X
        TARGET_USES_ICNSS_QMI := true
        CONFIG_ACS := true
        CONFIG_IEEE80211AC := true
        WIFI_DRIVER_FW_PATH_STA := "sta"
        WIFI_DRIVER_FW_PATH_AP  := "ap"
        WIFI_DRIVER_FW_PATH_P2P := "p2p"
        ifeq ($(call is-platform-sdk-version-at-least,17),true)
          # JB MR1 or later
          BOARD_HAS_CFG80211_KERNEL3_4 := true
          BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
          BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
        else
          # JB or earlier
          WIFI_CFG80211_DRIVER_MODULE_PATH := "/system/lib/modules/cfg80211.ko"
          WIFI_CFG80211_DRIVER_MODULE_NAME := "cfg80211"
          WIFI_CFG80211_DRIVER_MODULE_ARG  := ""
          WIFI_DRIVER_DEF_CONF_FILE := "/persist/WCNSS_qcom_cfg.ini"
          WIFI_DRIVER_CONF_FILE := "/data/misc/wifi/WCNSS_qcom_cfg.ini"
        endif
      endif
    endif   # !BUILD_TINY_ANDROID

    FEATURE_QCRIL_UIM_SAP_SERVER_MODE := true
endif #  !BOARD_PRESIL_BUILD
