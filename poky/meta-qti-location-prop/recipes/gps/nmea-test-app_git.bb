inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "NMEA test application "
PR = "r1"

SRC_URI = "file://gps/test/nmea_test_app/"
SRC_DIR = "${WORKSPACE}/gps/test/nmea_test_app/"
S = "${WORKDIR}/gps/test/nmea_test_app/"

DEPENDS = "qmi-framework loc-api-v02 loc-hal"
EXTRA_OECONF ="--with-glib"

