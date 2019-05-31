inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "Event Observer"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps/utils/eventObserver/"
SRC_DIR = "${WORKSPACE}/gps/utils/eventObserver/"
S = "${WORKDIR}/gps/utils/eventObserver"

DEPENDS = "gps-utils"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

