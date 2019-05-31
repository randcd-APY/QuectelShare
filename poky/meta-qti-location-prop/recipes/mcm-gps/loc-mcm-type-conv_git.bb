inherit autotools-brokensep pkgconfig qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "MCM Location Type Conversion Library"
PR = "r4"

FILESPATH        =+ "${WORKSPACE}:"
SRC_URI = "file://mcm-gps/loc-mcm-type-conv/"
SRC_DIR = "${WORKSPACE}/mcm-gps/loc-mcm-type-conv/"
S = "${WORKDIR}/mcm-gps/loc-mcm-type-conv"

DEPENDS = "mcm-core gps-utils"
EXTRA_OECONF = "--with-core-includes=${STAGING_INCDIR} \
                --with-glib"
