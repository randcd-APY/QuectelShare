inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc
DESCRIPTION = "GPS lowi test"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/internal/lowi/test/"
SRC_DIR = "${WORKSPACE}/gps-noship/internal/lowi/test/"
S = "${WORKDIR}/gps-noship/internal/lowi/test"

DEPENDS = "libxml2 loc-launcher lowi-client"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
