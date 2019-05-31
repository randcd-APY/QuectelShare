inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "GPS asn1c cper"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_DIR = "${WORKSPACE}/gps-noship-external/osys/asn1c/rtpersrc/"
SRC_URI = "file://gps-noship-external/osys/asn1c/rtpersrc/"
S = "${WORKDIR}/gps-noship-external/osys/asn1c/rtpersrc"
DEPENDS = "asn1c-crt"
EXTRA_OECONF = "--with-glib"
