inherit autotools-brokensep pkgconfig qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "MCM Location Server"
PR = "r5"

FILESPATH        =+ "${WORKSPACE}:"
SRC_URI = "file://mcm-gps/mcmlocserver/"
SRC_DIR = "${WORKSPACE}/mcm-gps/mcmlocserver/"
S = "${WORKDIR}/mcm-gps/mcmlocserver"

DEPENDS = "mcm loc-mcm-type-conv location-api location-service"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
