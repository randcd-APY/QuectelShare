inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "externaldr "
PR = "r1"

SRC_URI = "file://gps-noship/dr_amt/dr_glue/"
SRC_DIR = "${WORKSPACE}/gps-noship/dr_amt/dr_glue/"
S = "${WORKDIR}/gps-noship/dr_amt/dr_glue"

DEPENDS = "diag lbs-core loc-externaldrcore gnsspps"
EXTRA_OECONF = "--with-glib \
                --with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --enable-a6dof=yes \
                --enable-qdr2_customer1=no \
               "

