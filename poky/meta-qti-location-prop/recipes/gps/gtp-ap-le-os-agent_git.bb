inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc
DESCRIPTION = "GPS xtwifi LE os agent"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/xtwifi/gtp-ap-le-os-agent/"
SRC_DIR = "${WORKSPACE}/gps-noship/xtwifi/gtp-ap-le-os-agent/"
S = "${WORKDIR}/gps-noship/xtwifi/gtp-ap-le-os-agent"

DEPENDS = "loc-mq-client"
EXTRA_OECONF = "--with-glib"
