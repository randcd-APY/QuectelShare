inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "DR Plugin server"
PR = "r1"

SRC_URI = "file://gps-noship/dr_amt/drplugin_server/"
SRC_DIR = "${WORKSPACE}/gps-noship/dr_amt/drplugin_server/"
S = "${WORKDIR}/gps-noship/dr_amt/drplugin_server"

DEPENDS = "zlib loc-externaldr slim-client"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

