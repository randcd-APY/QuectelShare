inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "Izat Core"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/framework/native/core/"
SRC_DIR = "${WORKSPACE}/gps-noship/framework/native/core/"
S = "${WORKDIR}/gps-noship/framework/native/core"

# Depends are different based on qcmap is enabled or not
DEPENDS_QCMAP_ENABLED = "lbs-core-hdr loc-api-v02 loc-net-iface"
DEPENDS_QCMAP_DISABLED = "loc-hal lbs-core-hdr loc-api-v02 loc-base-util data-items dsutils gps-utils data"

DISABLE_QCMAP = 'False'
DISABLE_QCMAP = "${@bb.utils.contains('BASEMACHINE', '8x96autogvmquintcu', 'True', 'DISABLE_QCMAP',d)}"
# don't include loc-net-iface for 8x96autogvmquintcu
DEPENDS += "${@bb.utils.contains('DISABLE_QCMAP', 'True', '${DEPENDS_QCMAP_DISABLED}', '${DEPENDS_QCMAP_ENABLED}',d)}"

EXTRA_OECONF ="--with-glib \
               --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
               --with-lbscore-includes=${STAGING_INCDIR}/lbs-core-hdr \
               --enable-target=${BASEMACHINE}"

PACKAGES = "${PN}"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
FILES_${PN} = "${libdir}/*"
FILES_${PN} += "/usr/include/*"
FILES_${PN} += "/etc/*"

# The izat-core package contains symlinks that trip up insane
INSANE_SKIP_${PN} = "dev-so"
