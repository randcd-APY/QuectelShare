WLAN_CHIPSET := qca_cld3
PRODUCT_PACKAGES += $(WLAN_CHIPSET)_wlan.ko

PRODUCT_COPY_FILES += \
    device/qcom/wlan/msmnile/WCNSS_qcom_cfg.ini:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/WCNSS_qcom_cfg.ini \
				device/qcom/wlan/msmnile/wifi_concurrency_cfg.txt:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wifi_concurrency_cfg.txt \
				device/qcom/wlan/msmnile/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant_overlay.conf \
				device/qcom/wlan/msmnile/p2p_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/p2p_supplicant_overlay.conf
