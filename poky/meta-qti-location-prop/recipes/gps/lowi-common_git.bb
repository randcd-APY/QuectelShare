inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc
DESCRIPTION = "GPS lowi common"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/internal/lowi/common/"
SRC_DIR = "${WORKSPACE}/gps-noship/internal/lowi/common/"
S = "${WORKDIR}/gps-noship/internal/lowi/common/"

DEPENDS = "common libnl loc-mq-client"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
