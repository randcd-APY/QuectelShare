inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "GPS asn1c"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship-external/osys/asn1c/rtsrc/"
SRC_DIR = "${WORKSPACE}/gps-noship-external/osys/asn1c/rtsrc/"
S = "${WORKDIR}/gps-noship-external/osys/asn1c/rtsrc"

DEPENDS = "asn1c-rtx"
EXTRA_OECONF = "--with-glib"
