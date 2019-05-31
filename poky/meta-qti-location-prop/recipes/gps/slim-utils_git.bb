inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "SLIM Utils"
PR = "r1"

SRC_URI = "file://gps/slim/utils/"
SRC_DIR = "${WORKSPACE}/gps/slim/utils/"
S = "${WORKDIR}/gps/slim/utils"

DEPENDS = "diag qmi-framework gps-utils slim-common-hdr"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

