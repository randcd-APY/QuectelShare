inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "gnsspps"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://qcom-opensource/location/gnsspps/"
SRC_DIR = "${WORKSPACE}/qcom-opensource/location/gnsspps/"
S = "${WORKDIR}/qcom-opensource/location/gnsspps"
DEPENDS = "gps-utils"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
