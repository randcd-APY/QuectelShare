inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "SLIM Common library"
PR = "r1"

SRC_URI = "file://gps-noship/slim/"
SRC_DIR = "${WORKSPACE}/gps-noship/slim/"
S = "${WORKDIR}/gps-noship/slim"

DEPENDS = "slim-utils"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

