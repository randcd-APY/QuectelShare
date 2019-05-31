inherit autotools-brokensep pkgconfig qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "Engine Plugin api"
PR = "r1"

FILESPATH =+ "${WORKSPACE}/gps-noship:"
SRC_URI = "file://engine-plugin"
SRC_DIR = "${WORKSPACE}/gps-noship/engine-plugin"
DEPENDS = "glib-2.0 gps-utils diag engine-plugin-api-hdr engine-plugin-impl-hdr loc-eng-hub"
EXTRA_OECONF ="--with-glib"

S = "${WORKDIR}/engine-plugin"
PACKAGES = "${PN}"
FILES_${PN} = "${libdir}/*"
FILES_${PN} += "/usr/include/*"
FILES_${PN} += "/usr/bin/*"
INSANE_SKIP_${PN} = "dev-so"
