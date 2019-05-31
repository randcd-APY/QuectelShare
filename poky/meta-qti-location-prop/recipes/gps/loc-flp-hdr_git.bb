inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "flp common hdr"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/flp/"
SRC_DIR = "${WORKSPACE}/gps-noship/flp/"
S = "${WORKDIR}/gps-noship/flp"

do_configure() {
}

do_compile() {
}

do_install() {
    if [ -d "${SRC_DIR}" ]; then
    install -d ${D}${includedir}
    install -m 644 ${S}/fused_location_extended.h ${D}${includedir}
    else
    qprebuilt_do_install
    fi
}
