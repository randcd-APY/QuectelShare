inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "Lib DRPlugin"
PR = "r1"

SRC_URI = "file://gps-noship/dr_amt/drplugin/"
SRC_DIR = "${WORKSPACE}/gps-noship/dr_amt/drplugin/"
S = "${WORKDIR}/gps-noship/dr_amt/drplugin"

DEPENDS = "drplugin-client gnsspps"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

