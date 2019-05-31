inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "xtra daemon application "
PR = "r1"

SRC_URI = "file://gps-noship/xtra-daemon/"
SRC_DIR = "${WORKSPACE}/gps-noship/xtra-daemon/"
S = "${WORKDIR}/gps-noship/xtra-daemon/"

DEPENDS = "izat-core"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
