inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "GPS FLP"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/flp/"
SRC_DIR = "${WORKSPACE}/gps-noship/flp/"
S = "${WORKDIR}/gps-noship/flp"

DEPENDS = "location-geofence"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

PACKAGES = "${PN}"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
FILES_${PN} = "${libdir}/* ${sysconfdir}"
INSANE_SKIP_${PN} = "dev-so"
INSANE_SKIP_${PN}-dev += "dev-elf"
