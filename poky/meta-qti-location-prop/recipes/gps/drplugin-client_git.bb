inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "DR Plugin client"
PR = "r1"

SRC_URI = "file://gps-noship/dr_amt/drplugin_client/"
SRC_DIR = "${WORKSPACE}/gps-noship/dr_amt/drplugin_client/"
S = "${WORKDIR}/gps-noship/dr_amt/drplugin_client"

DEPENDS = "lbs-core libcutils"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

