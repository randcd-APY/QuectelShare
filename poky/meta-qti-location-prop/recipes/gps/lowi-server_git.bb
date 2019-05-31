inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc
DESCRIPTION = "GPS lowi server"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/internal/lowi/lowi_server/"
SRC_DIR = "${WORKSPACE}/gps-noship/internal/lowi/lowi_server/"
S = "${WORKDIR}/gps-noship/internal/lowi/lowi_server"

DEPENDS = "libnl loc-hal loc-launcher lowi-client"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
