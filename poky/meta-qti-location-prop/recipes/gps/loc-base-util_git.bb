inherit autotools-brokensep pkgconfig qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "LOC base utils"
PR = "r1"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps/framework/utils/native/base_util/"
SRC_DIR = "${WORKSPACE}/gps/framework/utils/native/base_util/"
S = "${WORKDIR}/gps/framework/utils/native/base_util"

DEPENDS = "glib-2.0 openssl sqlite3 loc-pla-hdr"
EXTRA_OECONF = "--with-glib"

do_install_append() {
    if [ -d "${D}/usr/include/loc-base-util" ]; then
        cp -R ${D}/usr/include/loc-base-util ${D}/usr/include/base_util
    fi
}
