inherit autotools-brokensep pkgconfig qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "GPS Data Items"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps/framework/native/lcp/data-items/"
SRC_DIR = "${WORKSPACE}/gps/framework/native/lcp/data-items/"
S = "${WORKDIR}/gps/framework/native/lcp/data-items"

DEPENDS = "common loc-core loc-base-util"
EXTRA_OECONF = "--with-libhardware-includes=${STAGING_INCDIR} \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-core-includes=${WORKSPACE}/system/core/include \
                --with-glib"

PACKAGES = "${PN}"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
FILES_${PN} = "${libdir}/*"
INSANE_SKIP_${PN} = "dev-so"
