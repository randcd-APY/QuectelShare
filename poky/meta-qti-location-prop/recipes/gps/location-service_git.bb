inherit autotools-brokensep pkgconfig qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "GPS Location Service library"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps/framework/native/"
SRC_DIR = "${WORKSPACE}/gps/framework/native/"
S = "${WORKDIR}/gps/framework/native"

DEPENDS = "data-items loc-mq-client lowi-client ulp2 loc-hal"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
