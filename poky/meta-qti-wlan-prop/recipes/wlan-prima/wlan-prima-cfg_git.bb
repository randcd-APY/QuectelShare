DESCRIPTION = "Qualcomm Atheros WLAN Firmware Configuration Files"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r0"

SRC_URI = "file://${WORKSPACE}/vendor/qcom/proprietary/wlan/prima/firmware_bin"

S = "${WORKDIR}/firmware_bin"

FILES_${PN} += "${base_libdir}/firmware/wlan/prima/*"

inherit autotools

do_install () {
    install -d ${D}${base_libdir}/firmware/wlan/prima
    install -m 0644 ${S}/* ${D}${base_libdir}/firmware/wlan/prima
}
