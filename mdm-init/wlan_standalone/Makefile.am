etcdir = ${sysconfdir}
dist_etc_DATA = cld-hostapd-wlan0.sh
dist_etc_DATA += cld-hostapd-wlan1.sh

misc_ddir = ${datadir}/misc/wifi
dist_misc_d_DATA = hostapd.conf
dist_misc_d_DATA += hostapd-wlan1.conf
dist_misc_d_DATA += sta_mode_hostapd.conf
dist_misc_d_DATA += wpa_supplicant.conf

init_ddir = ${sysconfdir}/init.d
dist_init_d_SCRIPTS = wlan

firmware_ddir = ${nonarch_base_libdir}/firmware/wlan
dist_firmware_d_DATA  = QCA6574AU.LE.2.2.1_Rome_PCIe_qcacld-2.0.ini
dist_firmware_d_DATA += QCA6574AU.LE.2.2.1_Rome_SDIO_qcacld-2.0.ini
dist_firmware_d_DATA += QCA6574AU.LE.2.2.1_Tufello_SDIO_qcacld-2.0.ini
dist_firmware_d_DATA += QCA6574AU.LE.2.2.2_Rome_SDIO_qcacld-2.0.ini
dist_firmware_d_DATA += QCA6574AU.LE.2.2.3_Rome_PCIe_qcacld-3.0.ini
dist_firmware_d_DATA += QCA6574AU.LE.2.6_Rome_PCIe_qcacld-3.0.ini

if NEED_SDIO_INI

# Copy qcom_cfg.sdio.ini to ../firmware/wlan/qca9377/
firmware_qca9377dir = ${nonarch_base_libdir}/firmware/wlan/qca9377
dist_firmware_qca9377_DATA = QCA6574AU.LE.2.2.1_Tufello_SDIO_qcacld-2.0.ini

firmware_qca6574dir = ${nonarch_base_libdir}/firmware/wlan/qca6574
dist_firmware_qca6574_DATA = QCA6574AU.LE.2.2.1_Rome_SDIO_qcacld-2.0.ini

# Rename qcom_cfg.sdio.ini to qcom_cfg.ini
install-data-hook:
	cd $(DESTDIR)$(firmware_qca9377dir) && \
		mv -f QCA6574AU.LE.2.2.1_Tufello_SDIO_qcacld-2.0.ini qcom_cfg.ini && \
	cd $(DESTDIR)$(firmware_qca6574dir) && \
		mv -f QCA6574AU.LE.2.2.1_Rome_SDIO_qcacld-2.0.ini qcom_cfg.ini

endif
