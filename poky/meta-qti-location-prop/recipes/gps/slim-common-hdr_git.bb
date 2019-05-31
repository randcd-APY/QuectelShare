inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "slim common hdr"
PR = "r1"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=3775480a712fc46a69647678acb234cb"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/slim/common/"
SRC_DIR = "${WORKSPACE}/gps-noship/slim/common/"
S = "${WORKDIR}/gps-noship/slim/common"

do_configure() {
}

do_compile() {
}

do_install() {
if [ -d "${SRC_DIR}" ]; then
    install -d ${D}/usr/include
    install -d ${D}/usr/include/libslimcommon
    install -m 644 ${S}/osal/inc/*.h ${D}/usr/include/libslimcommon
    install -m 644 ${S}/core/inc/*.h ${D}/usr/include/libslimcommon
    install -m 644 ${S}/client/inc/*.h ${D}/usr/include/libslimcommon
else
    qprebuilt_do_install

fi

}
