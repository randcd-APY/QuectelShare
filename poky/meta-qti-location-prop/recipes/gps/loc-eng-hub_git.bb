inherit autotools-brokensep pkgconfig qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "LOC Engine hub"
PR = "r1"

FILESPATH =+ "${WORKSPACE}/gps-noship:"
SRC_URI = "file://engine-hub"
SRC_DIR = "${WORKSPACE}/gps-noship/engine-hub"
DEPENDS = "loc-core lbs-core gnsspps diag"
EXTRA_OECONF ="--with-glib"

S = "${WORKDIR}/engine-hub"

PACKAGES = "${PN}"
FILES_${PN} = "${libdir}/*"
FILES_${PN} += "/usr/include/*"
INSANE_SKIP_${PN} = "dev-so"
