inherit autotools-brokensep pkgconfig qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "LOC MQ Client"
PR = "r1"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps/framework/utils/native/mq_client/"
SRC_DIR = "${WORKSPACE}/gps/framework/utils/native/mq_client/"
S = "${WORKDIR}/gps/framework/utils/native/mq_client"

DEPENDS = "gps-utils loc-base-util"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

do_install_append() {
    if [ -d "${D}/usr/include/loc-mq-client" ]; then
        cp -R ${D}/usr/include/loc-mq-client ${D}/usr/include/mq_client
    fi
}
