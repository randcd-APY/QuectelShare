inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "DR AP service"
PR = "r1"

SRC_URI = "file://gps-noship/dr_amt/DR_AP_service/"
SRC_DIR = "${WORKSPACE}/gps-noship/dr_amt/DR_AP_service/"
S = "${WORKDIR}/gps-noship/dr_amt/DR_AP_service"

DEPENDS = "drplugin-server"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

