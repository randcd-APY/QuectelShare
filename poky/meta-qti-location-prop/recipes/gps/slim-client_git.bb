inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "SLIM cleint "
PR = "r1"

SRC_URI = "file://gps/slim/client/"
SRC_DIR = "${WORKSPACE}/gps/slim/client/"
S = "${WORKDIR}/gps/slim/client"

DEPENDS = "qmi-framework gps-utils slim-utils slim-common"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
