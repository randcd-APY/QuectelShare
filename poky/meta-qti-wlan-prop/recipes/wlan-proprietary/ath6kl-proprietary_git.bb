inherit autotools

DESCRIPTION = "Qualcomm Atheros ath6kl utils and firmware"
HOMEPAGE = "http://support.cdmatech.com"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

DEPENDS = "libnl diag common glib-2.0 compat-wireless-ath6kl"

PR = "r5"

SRC_URI = "file://${WORKSPACE}/wlan-proprietary/target \
           file://${WORKSPACE}/wlan-proprietary/ath6kl-utils \
           "

S = "${WORKDIR}/ath6kl-utils"

EXTRA_OECONF = "--with-glib"

#TODO: Create a separate bb file for firmware.
MCKINLEY_SRC_FIRMWARE_DIR = "${WORKDIR}/target/AR6004/hw8.0/bin"
MCKINLEY_DEST_FIRMWARE_DIR = "${D}${base_libdir}/firmware/ath6k/AR6004/hw3.0"

do_install_append() {
    install -d ${MCKINLEY_DEST_FIRMWARE_DIR}

    install -m 0644 ${MCKINLEY_SRC_FIRMWARE_DIR}/* ${MCKINLEY_DEST_FIRMWARE_DIR}
}

FILES_${PN} += "${base_libdir}/firmware/ath6k/AR6004/hw3.0/*"
