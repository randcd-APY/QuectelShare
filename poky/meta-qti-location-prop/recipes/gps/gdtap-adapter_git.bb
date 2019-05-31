inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc
DESCRIPTION = "GPS xtwifi gdtap"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/gdtap/"
SRC_DIR = "${WORKSPACE}/gps-noship/gdtap/"
S = "${WORKDIR}/gps-noship/gdtap"

DEPENDS = "loc-launcher lbs-core"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
