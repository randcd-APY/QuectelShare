inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "Izat client api libary"
PR = "r1"

SRC_URI = "file://gps/framework/native/lcp/izat_api/"
SRC_DIR = "${WORKSPACE}/gps/framework/native/lcp/izat_api/"
S = "${WORKDIR}/gps/framework/native/lcp/izat_api"

DEPENDS = "izat-api-hdr location-service"
EXTRA_OECONF ="--with-glib \
               --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
               --with-izatapi-includes=${STAGING_INCDIR}/izat-api-hdr"

